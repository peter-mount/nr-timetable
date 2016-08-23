
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

struct ctx {
    char prefix;
    CharBuffer *b;
    List list;
};

static bool filter(void *v, void *c) {
    struct ctx *ctx = c;
    struct TTTiploc *t = v;

    return t->desc[0] == ctx->prefix;
}

static void next(void *c, void *v) {
    struct ctx *ctx = c;
    Node *n = node_alloc((char *) v);
    if (n)
        list_addTail(&ctx->list, n);
}

static int compare(Node *a, Node *b) {
    struct TTTiploc *ta = a->value;
    struct TTTiploc *tb = b->value;
    return strcmp(ta->desc, tb->desc);
}

static void *finish(void *c) {
    struct ctx *ctx = c;

    list_sort(&ctx->list, compare);

    // Write out our list
    charbuffer_add(ctx->b, '[');

    Node *n = list_getHead(&ctx->list);
    while (list_isNode(n)) {

        if (!list_isHead(n))
            charbuffer_add(ctx->b, ',');

        tt_append_tiploc(ctx->b, (struct TTTiploc *) n->value);

        n = list_getNext(n);
    }

    charbuffer_add(ctx->b, ']');

    // Free the list's Nodes - we must NOT remove the node names ie TTTiploc's
    n = list_getHead(&ctx->list);
    while (list_isNode(n)) {
        Node *next = list_getNext(n);
        n->value = NULL;
        node_free(n);
        n = next;
    }

    return NULL;
}

void tt_api_station(CharBuffer *b, const char *index) {
    logconsole("tt_api_station \"%s\"", index);
    if (!index || index[0] == 0)
        return;

    struct ctx ctx;
    ctx.prefix = toupper(*index);
    ctx.b = b;

    list_init(&ctx.list);

    Stream *s = hashmapStream(timetable->crsTiploc);
    if (s) {

        int r = stream_map(s, map_keyValue_value);

        if (!r) r = stream_filter(s, filter, &ctx, NULL);

        if (!r) r = stream_collect(s, NULL, next, finish, &ctx, NULL);

        if (r)
            stream_free(s);
        else
            stream_run(s, NULL);
    }
}
