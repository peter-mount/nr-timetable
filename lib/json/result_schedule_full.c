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
    if (!ctx->init) {
        charbuffer_append(ctx->b, "{\"schedule\":[");
        ctx->init = true;
    }
}

static void next(void *c, void *v) {
    if (c) {
        struct ctx *ctx = c;

        struct Schedule *s = (struct Schedule *) v;

        init(ctx);

        if (ctx->sep)
            charbuffer_add(ctx->b, ',');
        else
            ctx->sep = true;

        tt_append_schedule(ctx->b, s);

        // Add tiplocs to map
        mapTiploc_mapSchedule(ctx->tiploc, s);

        // Add activities to map
        ttref_add_schedule_activity(ctx->activity, s);
    }
}

static void *finish(void *c) {
    if (c) {
        struct ctx *ctx = c;

        init(ctx);

        charbuffer_append(ctx->b, "],\"tiploc\":{");
        if (ctx->tiploc)
            mapTiploc_appendIndex(ctx->b, ctx->tiploc);

        charbuffer_append(ctx->b, "},\"activity\":{");
        if (ctx->activity)
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
        ctx->sep = false;
        ctx->tiploc = mapTiploc_new();
        ctx->activity = hashmapCreate(10, hashmapStringHash, hashmapStringEquals);

        if (!stream_collect(s, NULL, next, finish, ctx, NULL))
            return EXIT_SUCCESS;

        freeCtx(ctx);
    }

    return EXIT_FAILURE;
}
