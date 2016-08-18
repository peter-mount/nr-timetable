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
 * Filter a stream of struct Schedule to include only schedules that operate
 * on a specific date
 * 
 * @param stream Stream to filter
 * @param t time_t of the required date (set to 00:00:00)
 */
int tt_filter_schedules_runson_date(Stream *s, time_t *t) {
    int r = tt_filter_schedule_contains_date(s, t);

    if (!r)
        r = tt_schedule_filter_runson_sameday(s, t);

    return r;
}
