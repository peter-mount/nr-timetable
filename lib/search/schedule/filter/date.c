#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>

/*
 * Stream filter to filter Schedules that contain the specified date witin
 * their ranges
 * 
 * time_t t;
 * stream_filter(stream, tt_schedule_filter_date, &t, NULL);
 * 
 * If t is null or 0 then all schedules are passed, otherwise only schedules
 * who's start <= t and end >= t;
 * 
 */
bool tt_schedule_filter_date(void *d, void *c) {
    if (!d)
        return false;

    if (!c)
        return true;

    struct Schedule *s = d;
    time_t *t = c;
    if (*t == 0)
        return true;

    return s->id.start <= *t && *t <= s->end;
}
