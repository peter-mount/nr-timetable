
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
    CharBuffer *b;
    Hashmap *map;
};

static void mapCrs(StreamData *d) {
    struct TTTiploc *t = stream_getVal(d);

    char *v = NULL;

    char *s = t->desc;

    if (!s[0]) s = t->nlcdesc;

    if (s[0]) {
        v = malloc(2);
        if (v) {
            v[0] = s[0];
            v[1] = 0;
        }
    }
    stream_setVal(d, v, v ? free : NULL);
}

static void next(void *c, void *v) {
    struct ctx *ctx = c;

    if (!hashmapContainsKey(ctx->map, v)) {
        char *value = strdup((char *) v);
        if (value)
            hashmapPut(ctx->map, value, value);
    }
}

static bool add(void *k, void *v, void *c) {
    Node *n = node_alloc((char *) v);
    if (n) {
        list_addTail((List *) c, n);
    }
    return true;
}

static int compare(Node *a, Node *b) {
    return strcmp(a->name, b->name);
}

static void *finish(void *c) {
    struct ctx *ctx = c;

    List list;
    list_init(&list);

    // Add all values to the list
    hashmapForEach(ctx->map, add, &list);

    // Sort them
    list_sort(&list, compare);

    // Write out our list
    charbuffer_add(ctx->b, '[');

    Node *n = list_getHead(&list);
    while (list_isNode(n)) {

        if (!list_isHead(n))
            charbuffer_add(ctx->b, ',');

        json_append_str(ctx->b, n->name);

        n = list_getNext(n);
    }

    charbuffer_add(ctx->b, ']');

    // Free the list's Nodes - removing our copies of the node names
    n = list_getHead(&list);
    while (list_isNode(n)) {
        Node *next = list_getNext(n);
        hashmapRemove(ctx->map, n->name);
        node_free(n);
        n = next;
    }

    return NULL;
}

/*
 * Returns a json array of letters, one each for the start of each location
 * thats in the crsTiploc hashmap.
 * 
 * This will always be accurate for whatever tiploc's exist, so will only
 * include letters that exist in the hashmap
 */
void tt_api_station_index(CharBuffer *b) {

    struct ctx ctx;
    ctx.b = b;
    ctx.map = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    if (!ctx.map)
        return;

    Stream *s = hashmapStream(timetable->crsTiploc);
    if (s) {

        int r = stream_map(s, map_keyValue_value);

        if (!r) r = stream_map(s, mapCrs);

        if (!r) r = stream_notNull(s);

        if (!r) r = stream_collect(s, NULL, next, finish, &ctx, NULL);

        if (r)
            stream_free(s);
        else
            stream_run(s, NULL);
    }

    hashmapFree(ctx.map);
}
