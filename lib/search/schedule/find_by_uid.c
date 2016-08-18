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
    
    hashmapGetMapper(stream, timetable->uid);
    stream_notNull(stream);

    // flatMap the list to nodes
    stream_flatMap(stream, list_flatMap, NULL);

    // Get Schedule from node->name
    list_map_node_name(stream);
    return stream;
}
