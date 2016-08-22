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
#include <area51/list.h>
#include <area51/log.h>
#include <area51/stream.h>

/*
 * Creates a stream that returns all schedules for a specific uid
 */
Stream *tt_search_schedules_by_uid(char *uid) {
    Stream *stream = stream_of(uid, NULL);
    if (!stream)
        return NULL;

    int r = hashmapGetMapper(stream, timetable->uid);
    if (!r)
        r = stream_notNull(stream);

    // flatMap the list to nodes
    if (!r)
        r = list_flatMap(stream);

    // Get Schedule from node->name
    if (!r)
        r = list_map_node_name(stream);

    if (r) {
        stream_free(stream);
        return NULL;
    }

    return stream;
}

static Stream *flatMap(StreamData *d, void *c) {
    char *uid = stream_getVal(d);
    return tt_search_schedules_by_uid(uid);
}

/*
 * Enable tt_search_schedules_by_uid to also be used in a stream
 */
int tt_flatMap_schedules_by_uid(Stream *s) {
    return stream_flatMap(s, flatMap, NULL);
}
