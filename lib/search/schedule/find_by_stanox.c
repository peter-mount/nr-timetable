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
 * Creates a stream that returns all schedules for a specific stanox
 */
Stream *tt_search_schedules_by_stanox(int stanox) {
    Stream *stream = stream_of_int(stanox);

    return stream;
}
