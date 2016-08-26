
#include <pthread.h>
#include <microhttpd.h>
#include <stdint.h>
#include <area51/webserver.h>

#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>

#include <area51/hashmap.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/stream.h>
#include <microhttpd.h>

struct ctx {
    char *pattern;
    int len;
    CharBuffer *b;
};

static bool filter(void *v, void *c) {
    struct ctx *ctx = c;
    struct TTTiploc *t = v;

    // This will filter out tiplocs with a crs but are not a station
    if (!t->nlcdesc[0])
        return false;

    if (ctx->len == 3 && strncmp(t->crs, ctx->pattern, 3) == 0)
        return true;

    if (ctx->len <= TIPLOC_LENGTH && strncmp(t->tiploc, ctx->pattern, ctx->len) == 0)
        return true;

    return strstr(t->desc, ctx->pattern) != NULL;
}

static void *finish(List *l, void *c) {
    struct ctx *ctx = c;

    charbuffer_add(ctx->b, '[');

    Node *n = list_getHead(l);
    while (list_isNode(n)) {

        if (!list_isHead(n))
            charbuffer_add(ctx->b, ',');

        tt_append_tiploc(ctx->b, (struct TTTiploc *) n->value);

        n = list_getNext(n);
    }

    charbuffer_add(ctx->b, ']');
    return NULL;
}

static void search(CharBuffer *b, const char *key) {
    struct ctx ctx;
    memset(&ctx, 0, sizeof (struct ctx));
    ctx.b = b;

    if (key) {
        char *p = (char *) key;

        while (*p && *p == ' ')
            p++;

        while (*p)
            charbuffer_add(b, toupper(*p++));

        ctx.pattern = charbuffer_tostring(b, &ctx.len);
    }

    charbuffer_reset(b);

    if (ctx.len >= 3) {
        Stream *s = hashmapStream(timetable->crsTiploc);
        if (s) {

            int r = stream_map(s, map_keyValue_value);

            if (!r) r = stream_filter(s, filter, &ctx, NULL);

            if (!r) r = stream_limit(s, 10);

            if (!r) r = tt_collect_tiploc(s, finish, &ctx, NULL);

            if (r)
                stream_free(s);
            else
                stream_run(s, NULL);
        }
    } else
        charbuffer_append(b, "[]");
}

/**
 * Handles searching stations by key name.
 * 
 * This is one of our few API's that accepts a query string - mainly to support
 * jQuery autocomplete components.
 */
int tt_api_station_search(WEBSERVER_REQUEST *request) {

    CharBuffer b;
    charbuffer_init(&b);

    search(&b, webserver_getRequestParameter(request, "term"));

    struct MHD_Response *response = MHD_create_response_from_buffer(b.pos, b.buffer, MHD_RESPMEM_MUST_FREE);
    return webserver_queueResponse(request, &response);
}
