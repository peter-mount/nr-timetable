#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>

/*
 * Stream filter to filter Schedules so it ignores those that are not running
 * on a specific day of the week.
 * 
 * Stream *stream;
 * time_t t;
 * tt_schedule_filter_dayRunning(stream, &t);
 * 
 */
static bool filter(void *d, void *c) {
    if (!d)
        return false;

    struct Schedule *s = d;
    unsigned int *mask = c;

    return s->daysRun & *mask;
}

int tt_schedule_filter_dayRunning(Stream *s, time_t *t) {
    unsigned int *mask = malloc(sizeof (unsigned int));
    if (!mask)
        return EXIT_FAILURE;

    struct tm tm;
    memset(&tm, 0, sizeof (struct tm));
    gmtime_r(t, &tm);

    // struct tm sunday is 0 but CIF 0 is Monday so adjust
    int dow = tm.tm_wday - 1;
    if (dow == -1)
        dow = 6;

    *mask = 1 << dow;

    if (stream_filter(s, filter, mask, free)) {
        free(mask);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
