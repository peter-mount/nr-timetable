#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <area51/charbuffer.h>
#include <area51/comparator.h>
#include <area51/json.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/rest.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>

struct ctx {
    CharBuffer *b;
    List list;
    Hashmap *tiploc;
    Hashmap *activity;
};

static void freeNode(Node *n) {
    if (n) {
        // We mustn't free the underlying Schedule
        n->value = NULL;

        node_free(n);
    }
}

/*
 * Comparator to sort by UID
 * 
 * This is used to remove duplicates
 */
static char *STP = "CNOP";

static int uidComparator(Node *a, Node *b) {
    struct Schedule *ai = a->value;
    struct Schedule *bi = b->value;

    int r = strcmp(ai->id.uid, bi->id.uid);

    // Sort by start
    if (!r) r = comparator_time_t(&ai->id.start, &bi->id.start);

    // If equal sort by STP
    if (!r) r = comparator_long(
            (long *) strchrnul(STP, ai->id.stpInd),
            (long *) strchrnul(STP, ai->id.stpInd)
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
    list_sort(list, uidComparator);

    // Remove duplicates
    struct ScheduleId *id = NULL;
    Node *n = list_getHead(list);
    while (list_isNode(n)) {
        Node *next = list_getNext(n);

        struct Schedule *s = n->value;
        if (id && memcmp(id, &s->id, sizeof (struct ScheduleId)) == 0) {
            list_remove(n);
            freeNode(n);
        } else
            id = &s->id;

        n = next;
    }
}

static void freeCtx(struct ctx *ctx) {
    if (ctx) {
        Node * n = list_getHead(&ctx->list);
        while (list_isNode(n)) {
            Node *next = list_getNext(n);
            list_remove(n);
            freeNode(n);
            n = next;
        }
        if (ctx->tiploc)
            hashmapFree(ctx->tiploc);

        if (ctx->activity)
            hashmapFree(ctx->activity);

        free(ctx);
    }
}

static void next(void *c, void *v) {
    if (c) {
        struct ctx *ctx = c;

        struct Schedule *s = (struct Schedule *) v;

        Node *n = node_alloc((char *) s);
        if (n)
            list_addTail(&ctx->list, n);
    }
}

static void *finish(void *c) {
    if (c) {
        struct ctx *ctx = c;

        // Remove duplicates
        removeDuplicates(&ctx->list);

        charbuffer_append(ctx->b, "{\"schedule\":[");
        Node * n = list_getHead(&ctx->list);
        while (list_isNode(n)) {
            struct Schedule *s = n->value;

            if (!list_isHead(n))
                charbuffer_add(ctx->b, ',');

            tt_append_schedule(ctx->b, s);

            // Add tiplocs to map
            mapTiploc_mapSchedule(ctx->tiploc, s);

            // Add activities to map
            ttref_add_schedule_activity(ctx->activity, s);

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
int tt_schedule_result_full(Stream *s, CharBuffer *b) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (ctx) {
        memset(ctx, 0, sizeof (struct ctx));
        ctx->b = b;
        list_init(&ctx->list);
        ctx->tiploc = mapTiploc_new();
        ctx->activity = hashmapCreate(10, hashmapStringHash, hashmapStringEquals);

        if (!stream_collect(s, NULL, next, finish, ctx, NULL))
            return EXIT_SUCCESS;

        freeCtx(ctx);
    }

    return EXIT_FAILURE;
}
