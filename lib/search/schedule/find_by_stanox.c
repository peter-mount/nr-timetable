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

/*
 * Creates a stream that returns all schedules that run through a specific stanox
 */
Stream *tt_search_schedules_by_stanox(int stanox) {
    int *v = malloc(sizeof (int));
    if (!v)
        return NULL;

    // Stream of stanox->scheduleId (int)
    *v = stanox;
    Stream *stream = stream_of(v, free);
    if (!stream) {
        free(v);
        return NULL;
    }

    // Lookup in index
    int r = hashmapGetMapper(stream, timetable->schedStanox);

    if (!r)
        r = stream_notNull(stream);

    // flatMap so we now have stream of int scheduleID's
    if (!r)
        r = tt_stanox_schedule_flatMap(stream);

    // map to string schedule UID
    if (!r)
        r = hashmapGetMapper(stream, timetable->schedId);

    // flatmap to stream of struct Schedule
    if (!r)
        r = tt_flatMap_schedules_by_uid(stream);

    // At this point we now have a stream of struct Schedule at this location

    if (r) {
        stream_free(stream);
        return NULL;
    }
    return stream;
}
