#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/stream.h>

struct ctx {
    CharBuffer *b;
    bool init;
    bool sep;
    int stanox;
    Hashmap *tiploc;
    Hashmap *activity;
};

static void freeCtx(struct ctx *ctx) {
    if (ctx) {
        if (ctx->tiploc)
            hashmapFree(ctx->tiploc);

        if (ctx->activity)
            hashmapFree(ctx->activity);

        free(ctx);
    }
}

static void init(struct ctx *ctx) {
    if (ctx->init) {
        charbuffer_append(ctx->b, "{\"schedule\":[");
        ctx->init = false;
    }
}

static void appendTpl(struct ctx *ctx, struct ScheduleEntry *e, char *label) {
    charbuffer_append(ctx->b, ",\"");
    charbuffer_append(ctx->b, label);
    charbuffer_append(ctx->b, "\":{");
    tt_append_tiploc_field(ctx->b, "tiploc", e->time.tiploc);
    //charbuffer_add(ctx->b, ',');
    charbuffer_add(ctx->b, '}');

    // Add tiploc to reference
    mapTiploc_mapScheduleEntry(ctx->tiploc, e);

    // Add activity
    ttref_add_activity(ctx->activity, e->time.activity);
}

static void append(struct ctx *ctx, struct Schedule *s, struct ScheduleEntry *entries, int index) {
    if (ctx->sep)
        charbuffer_add(ctx->b, ',');
    else
        ctx->sep = true;

    charbuffer_append(ctx->b, "{\"uid\":");
    json_append_str(ctx->b, s->id.uid);

    charbuffer_append(ctx->b, ",\"stpInd\":\"");
    charbuffer_add(ctx->b, s->id.stpInd);
    charbuffer_add(ctx->b, '"');

    if (entries) {
        appendTpl(ctx, &entries[0], "origin");
        appendTpl(ctx, &entries[s->numEntries - 1], "dest");
        appendTpl(ctx, &entries[index], "loc");
    }

    charbuffer_add(ctx->b, '}');
}

static void next(void *c, void *v) {
    if (c) {
        struct ctx *ctx = c;
        CharBuffer *b = ctx->b;
        struct Schedule *s = (struct Schedule *) v;
        struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->sid);

        init(ctx);

        // Loop and find our stanox. Do the entire schedule as a stanox can exist
        // multiple times on circular & long schedules.
        if (entries)
            for (int i = 0; i < s->numEntries; i++) {
                short tiploc = entries[i].time.tiploc;
                struct TTTiploc *tpl = hashmapGet(timetable->idTiploc, &tiploc);
                if (tpl && tpl->stanox == ctx->stanox)
                    append(ctx, s, entries, i);
            }
    }
}

static void *finish(void *c) {
    if (c) {
        struct ctx *ctx = c;

        init(ctx);

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
int tt_schedule_result_index(Stream *s, CharBuffer *b, int stanox) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (ctx) {
        memset(ctx, 0, sizeof (struct ctx));
        ctx->b = b;
        ctx->sep = false;
        ctx->init = true;
        ctx->stanox = stanox;
        ctx->tiploc = mapTiploc_new();
        ctx->activity = hashmapCreate(10, hashmapStringHash, hashmapStringEquals);

        if (!stream_collect(s, NULL, next, finish, ctx, NULL))
            return EXIT_SUCCESS;

        freeCtx(ctx);
    }

    return EXIT_FAILURE;
}
