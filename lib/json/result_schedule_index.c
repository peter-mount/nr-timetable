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
    bool sep;
    Hashmap *tiploc;
    Hashmap *activity;
};

static void init(void *c) {
    struct ctx *ctx = c;
    if (ctx)
        charbuffer_append(ctx->b, "{\"schedule\":[");
}

static void next(void *c, void *v) {
    if (c) {
        struct ctx *ctx = c;

        struct Schedule *s = (struct Schedule *) v;

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

        charbuffer_append(ctx->b, "],\"tiploc\":{");
        if (ctx->tiploc)
            mapTiploc_appendIndex(ctx->b, ctx->tiploc);

        charbuffer_append(ctx->b, "},\"activity\":{");
        if (ctx->activity)
            ttref_print_activity_index(ctx->b, ctx->activity);

        charbuffer_append(ctx->b, "}}");

        if (ctx->tiploc)
            hashmapFree(ctx->tiploc);

        if (ctx->activity)
            hashmapFree(ctx->activity);

        free(ctx);
    }

    return NULL;
}

/*
 * Stream collector that will add a schedule index json.
 * 
 * This lists the schedule, date and time at a specific location.
 */
int tt_schedule_result_index(Stream *s, CharBuffer *b) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (ctx) {
        ctx->b = b;
        ctx->sep = false;
        ctx->tiploc = mapTiploc_new();
        ctx->activity = hashmapCreate(10, hashmapStringHash, hashmapStringEquals);

        if (!stream_collect(s, init, next, finish, ctx, free))
            return EXIT_SUCCESS;

        free(ctx);
    }

    return EXIT_FAILURE;
}
