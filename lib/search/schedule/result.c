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

struct ctx {
    struct charbuffer *b;
    bool sep;
    Hashmap *tiploc;
    Hashmap *activity;
    bool(*filter)(struct Node *n, void *c);
};

static bool forEach(struct Node *n, void *c) {
    struct ctx *ctx = (struct ctx *) c;

    if (ctx->filter == NULL || ctx->filter(n, c)) {
        struct Schedule *s = (struct Schedule *) n->name;

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

    return true;
}

/*
 * Handles the common response format for a schedule search.
 * 
 * @param b charbuffer to write to
 * @param l List containing Schedules
 * @param filter optional filter to run against each result
 * @oaran c context to pass to the filter
 */
void tt_schedules_result(struct charbuffer *b, struct List *l, bool(*filter)(struct Node *n, void *c), void *c) {
    charbuffer_append(b, "{\"schedule\":[");
    //json_append_list(b, l, tt_append_schedule_node);

    if (l && !list_isEmpty(l)) {
        struct ctx ctx;
        ctx.b = b;
        ctx.sep = false;
        ctx.filter = filter;

        ctx.tiploc = mapTiploc_new();
        if (!ctx.tiploc)
            return;

        ctx.activity = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
        if (!ctx.activity) {
            hashmapFree(ctx.tiploc);
            return;
        }

        list_forEach(l, forEach, &ctx);

        charbuffer_append(b, "],\"tiploc\":{");
        mapTiploc_appendIndex(b, ctx.tiploc);
        charbuffer_add(b, '}');

        charbuffer_append(b, ",\"activity\":{");
        ttref_print_activity_index(b, ctx.activity);
        charbuffer_add(b, '}');

        hashmapFree(ctx.tiploc);
        hashmapFree(ctx.activity);
    }

    charbuffer_add(b, '}');
}
