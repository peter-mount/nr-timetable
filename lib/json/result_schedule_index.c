#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <area51/comparator.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/rest.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>

// collector context data

struct ctx {
    int stanox;
    int hour;
    bool filterHour;
    CharBuffer *b;
    List list;
    Hashmap *tiploc;
    Hashmap *activity;
};

// The data we are interested in

struct ScheduleIndex {
    struct ScheduleId *id;
    struct ScheduleTime *origin;
    struct ScheduleTime *dest;
    struct ScheduleTime *loc;
};

/*
 * Comparator to sort ScheduleIndex by loc.pta or loc.wta (if a pass).
 * 
 * This is used to sort the result into time order
 */

static int timeComparator(Node *a, Node *b) {
    int ta = scheduleTime_getTime(((struct ScheduleIndex *) a->value)->loc);
    int tb = scheduleTime_getTime(((struct ScheduleIndex *) b->value)->loc);

    return ta < tb ? -1 : ta > tb ? 1 : 0;
}

/*
 * Comparator to sort by UID, tiploc, tiplocseq and stpInd
 * 
 * This is used to remove duplicates
 */
static char *STP = "CNOP";

static int duplicateComparator(Node *a, Node *b) {
    struct ScheduleIndex *ai = a->value;
    struct ScheduleIndex *bi = b->value;

    int r = strcmp(ai->id->uid, bi->id->uid);

    // Sort by start
    if (!r) r = comparator_time_t(&ai->id->start, &bi->id->start);

    // Sort by tiploc
    if (!r) {
        int ta = ai->loc->tiploc;
        int tb = bi->loc->tiploc;
        r = comparator_int(&ta, &tb);
    }

    // Sort by tiplocseq
    if (!r) {
        int ta = ai->loc->tiplocseq;
        int tb = bi->loc->tiplocseq;
        r = comparator_int(&ta, &tb);
    }

    // If equal sort by STP
    if (!r) r = comparator_long(
            (long *) strchrnul(STP, ai->id->stpInd),
            (long *) strchrnul(STP, ai->id->stpInd)
            );

    return r;
}

/*
 * Remove any duplicate schedules. We do this by sorting against ScheduleID
 * and then keeping the first one, which should match the order in STP above.
 * 
 * This shouldn't be necessary if the underlying stream filtered correctly
 * but this is a second check, more so if like the stanox API we can't guarantee
 * uniqueness until streams support this (which may be some time).
 */
static void removeDuplicates(List *list) {
    // Sort into UID order
    list_sort(list, duplicateComparator);

    // Remove duplicates
    char *uid = NULL;
    int tiploc = 0;
    int seq = 0;
    Node *n = list_getHead(list);
    while (list_isNode(n)) {
        Node *next = list_getNext(n);

        struct ScheduleIndex *idx = n->value;

        if (!list_isHead(n) &&
                strcmp(uid, idx->id->uid) == 0 &&
                tiploc == idx->loc->tiploc && seq == idx->loc->tiplocseq
                ) {
            // This will remove & free n and the ScheduleIndex instance
            list_remove(n);
            node_free(n);
        } else {
            uid = idx->id->uid;
            tiploc = idx->loc->tiploc;
            seq = idx->loc->tiplocseq;
        }

        n = next;
    }
}

/*
 * Frees ctx and all entries within it
 */
static void freeCtx(struct ctx *ctx) {
    if (ctx) {
        Node *n = list_getHead(&ctx->list);
        while (list_isNode(n)) {
            Node *next = list_getNext(n);
            list_remove(n);
            // This will free n and the ScheduleIndex instance
            node_free(n);
            n = next;
        }

        if (ctx->tiploc)
            hashmapFree(ctx->tiploc);

        if (ctx->activity)
            hashmapFree(ctx->activity);

        free(ctx);
    }
}

/*
 * Appends a named entry into the json. This is either origin, dest or loc
 */
static void appendTpl(struct ctx *ctx, struct ScheduleTime *t, char *label) {
    charbuffer_append(ctx->b, ",\"");
    charbuffer_append(ctx->b, label);
    charbuffer_append(ctx->b, "\":{");
    tt_append_scheduleTime(ctx->b, t);
    charbuffer_add(ctx->b, '}');

    // Add tiploc to reference
    mapTiploc_addTiploc(ctx->tiploc, t->tiploc);

    // Add activity
    ttref_add_activity(ctx->activity, t->activity);
}

/*
 * Appends a ScheduleIndex to the result.
 * 
 * @param s Schedule
 * @param entries ScheduleEntry array for this schedule
 * @param index index in entries of the entry for the required stanox
 */
static void appendEntry(struct ctx *ctx, struct Schedule *s, struct ScheduleEntry *entries, int index) {
    struct ScheduleIndex *i = malloc(sizeof (struct ScheduleIndex));
    if (i) {
        i->id = &s->id;
        i->origin = &entries[0].time;
        i->dest = &entries[s->numEntries - 1].time;
        i->loc = &entries[index].time;

        Node *n = node_alloc((char*) i);
        if (!n)
            free(i);
        else
            list_addTail(&ctx->list, n);
    }
}

/*
 * Accepts value into the collector.
 * 
 * This will parse the Schedule for any entries for the stanox of interest and
 * add a ScheduleIndex entry into the result
 */
#define getHour(ctx,time) ((ctx)->hour==((scheduleTime_getTime(&entries[i].time) / 3600) % 24))

static void next(void *c, void *v) {
    if (c) {
        struct ctx *ctx = c;
        CharBuffer *b = ctx->b;
        struct Schedule *s = (struct Schedule *) v;
        struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->sid);

        // Loop and find our stanox. Do the entire schedule as a stanox can exist
        // multiple times on circular & long schedules.
        if (entries)
            for (int i = 0; i < s->numEntries; i++) {
                short tiploc = entries[i].time.tiploc;
                struct TTTiploc *tpl = hashmapGet(timetable->idTiploc, &tiploc);
                if (tpl && tpl->stanox == ctx->stanox
                        && (!ctx->filterHour || ctx->hour == ((scheduleTime_getTime(&entries[i].time) / 3600) % 24))
                        )
                    appendEntry(ctx, s, entries, i);
            }
    }
}

static void *finish(void *c) {
    if (c) {
        struct ctx *ctx = c;

        // First remove any duplicates
        removeDuplicates(&ctx->list);

        // Sort result into time order
        list_sort(&ctx->list, timeComparator);

        // Write the schedules
        charbuffer_append(ctx->b, "{\"schedule\":[");
        Node *n = list_getHead(&ctx->list);
        while (list_isNode(n)) {
            struct ScheduleIndex *idx = n->value;

            if (!list_isHead(n))
                charbuffer_add(ctx->b, ',');

            // Schedule details
            charbuffer_append(ctx->b, "{\"uid\":");
            json_append_str(ctx->b, idx->id->uid);

            charbuffer_append(ctx->b, ",\"stpInd\":\"");
            charbuffer_add(ctx->b, idx->id->stpInd);
            charbuffer_add(ctx->b, '"');

            // Display time
            append_hhmmss(ctx->b, "time", scheduleTime_getTime(idx->loc));

            // Location details
            appendTpl(ctx, idx->origin, "origin");
            appendTpl(ctx, idx->dest, "dest");
            appendTpl(ctx, idx->loc, "loc");

            // Type flags, only include if true, one of public, working or pass
            if (idx->loc->pta || idx->loc->ptd)
                charbuffer_append(ctx->b, ",\"public\":true");
            else if (idx->loc->wta || idx->loc->wtd)
                charbuffer_append(ctx->b, ",\"working\":true");
            else
                charbuffer_append(ctx->b, ",\"pass\":true");

            charbuffer_add(ctx->b, '}');

            n = list_getNext(n);
        }

        charbuffer_append(ctx->b, "],\"tiploc\":{");
        mapTiploc_appendIndex(ctx->b, ctx->tiploc);

        charbuffer_append(ctx->b, "},\"activity\":{");
        ttref_print_activity_index(ctx->b, ctx->activity);

        charbuffer_append(ctx->b, "}}");

        freeCtx(ctx);
    }

    return NULL;
}

/*
 * Stream collector that will add Schedules to the standard full result json.
 * 
 * This json contains a list of schedules in their entirety plus the
 * regular tiploc and activity lookup tables
 * 
 */
int tt_schedule_result_index(Stream *s, CharBuffer *b, int stanox, int hour, bool filterHour) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (ctx) {
        memset(ctx, 0, sizeof (struct ctx));
        ctx->b = b;
        list_init(&ctx->list);
        ctx->stanox = stanox;
        ctx->hour = hour;
        ctx->filterHour = filterHour;
        ctx->tiploc = mapTiploc_new();
        ctx->activity = hashmapCreate(10, hashmapStringHash, hashmapStringEquals);

        if (!stream_collect(s, NULL, next, finish, ctx, NULL))
            return EXIT_SUCCESS;

        freeCtx(ctx);
    }

    return EXIT_FAILURE;
}
