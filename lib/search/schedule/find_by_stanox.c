#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <area51/hashmap.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/stream.h>

extern int tt_stanox_schedule_flatMap(Stream *);

static void dbb(StreamData *d) {
    logconsole("Value %lx", stream_getVal(d));
    stream_next(d);
}

static int db(Stream *s) {
    return stream_invoke(s, dbb, NULL, NULL);
}
/*
 * Creates a stream that returns all schedules for a specific stanox
 */
Stream *tt_search_schedules_by_stanox(int stanox) {
    int *v = malloc(sizeof (int));
    if (!v)
        return NULL;

    *v = stanox;
    Stream *stream = stream_of(v, free);
    if (!stream) {
        free(v);
        return NULL;
    }

    int r = hashmapGetMapper(stream, timetable->schedStanox);

    db(stream);

    if (!r)
        r = stream_notNull(stream);

    db(stream);

    // flatMap the list to give us a stream of streams at this location
    if (!r)
        //r = list_flatMap(stream);
        r = tt_stanox_schedule_flatMap(stream);

    db(stream);

    if (!r)
        r = list_map_node_name(stream);

    db(stream);

    if (!r)
        r = hashmapGetMapper(stream, timetable->schedId);

    if (r) {
        stream_free(stream);
        return NULL;
    }
    return stream;
}
