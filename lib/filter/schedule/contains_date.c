#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>

static bool filter(void *d, void *c) {
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

/*
 * Filter a stream of struct Schedule to contain schedules that contain a
 * specific date.
 * 
 * @param s Stream
 * @param t time_t to filter against
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
int tt_filter_schedule_contains_date(Stream *s, time_t *t) {
    return stream_filter(s, filter, t, NULL);
}