
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
    List list;
    void *ctx;
    void (*free)(void *);
    void *(*finish)(List *, void *);
};

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

    return ctx->finish(&ctx->list, ctx->ctx);
}

static void freeCtx(void *c) {
    if (c) {
        struct ctx *ctx = c;
        // Free the list's Nodes - we must NOT remove the node names ie TTTiploc's
        Node *n = list_getHead(&ctx->list);
        while (list_isNode(n)) {
            Node *next = list_getNext(n);
            n->value = NULL;
            node_free(n);
            n = next;
        }

        if (ctx->ctx && ctx->free)
            ctx->free(ctx->ctx);

        free(ctx);
    }
}

/**
 * Collect TTTiploc's, sort by desc and then pass to a handler on completion
 * @param s Stream
 * @param finisher Finisher
 * @param c Context for finisher
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int tt_collect_tiploc(Stream *s, void *(*finisher)(List *, void *), void *c, void (*f)(void *)) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (!ctx)
        return EXIT_FAILURE;

    memset(ctx, 0, sizeof (struct ctx));
    list_init(&ctx->list);
    ctx->finish = finisher;
    ctx->ctx = c;
    ctx->free = f;

    int r = stream_collect(s, NULL, next, finish, ctx, freeCtx);
    if (r)
        freeCtx(ctx);
    return r;
}
