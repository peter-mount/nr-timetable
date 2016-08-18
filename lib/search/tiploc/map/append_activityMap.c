
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>

#include <area51/list.h>
#include <area51/log.h>

struct ctx {
    CharBuffer *b;
    bool sep;
};

static bool append(void *k, void *v, void *c) {
    struct ctx *ctx = (struct ctx *) c;
    struct TTTiploc *tpl = (struct TTTiploc *) v;
    if (ctx->sep)
        charbuffer_add(ctx->b, ',');
    else
        ctx->sep = true;
    charbuffer_add(ctx->b, '\"');
    charbuffer_append(ctx->b, tpl->tiploc);
    charbuffer_append(ctx->b, "\":");
    tt_append_tiploc(ctx->b, tpl);

    return true;
}

/*
 * Append the contents of a Hashmap (values are struct TTTiploc) to an existing
 * JSON object.
 * 
 * Note: The keys are ignored so can be anything, just the values must be correct.
 * 
 */
void mapTiploc_appendIndex(CharBuffer *b, Hashmap *m) {
    struct ctx ctx;
    ctx.b = b;
    ctx.sep = false;
    hashmapForEach(m, append, &ctx);
}
