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
    unsigned int mask = stream_void_int(c);

    return s->daysRun & mask;
}

/*
 * Filters a stream of struct Schedule to contain only schedules that run on
 * the specified day's of the week.
 * 
 * The supplied value is a 7-bit mask, each bit representing a day of the week.
 * 
 * Monday is bit 0, Tuesday bit 1 etc with Sunday bit 6.
 * 
 * So to filter on specific days:
 * 
 * ----------------------
 *   | Day of week | dow
 * ----------------------
 * 0 | Monday      |   1
 * 1 | Tuesday     |   2
 * 2 | Wednesday   |   4
 * 3 | Thursday    |   8
 * 4 | Friday      |  16
 * 5 | Saturday    |  32
 * 6 | Sunday      |  64
 *   | Mon - Fri   |  31
 *   | Sat - Sun   |  96
 *   | All week    | 127
 * ----------------------
 * 
 * @param s Stream
 * @param dow days of week
 */
int tt_schedule_filter_runson_daysofweek(Stream *s, int dow) {
    return stream_filter(s, filter, stream_int_void(dow), NULL);
}

/*
 * Filters a stream of struct Schedule to contain only schedules that run on
 * the same day of the week as the supplied time_t.
 * 
 * This filter only handles the day of the week, not the specific date. You
 * should use other filters to further filter the result.
 * 
 * @param s Stream
 * @param t time_t to get the day of week
 */
int tt_schedule_filter_runson_sameday(Stream *s, time_t *t) {
    unsigned int mask;
    struct tm tm;
    memset(&tm, 0, sizeof (struct tm));
    gmtime_r(t, &tm);

    // struct tm sunday is 0 but CIF 0 is Monday so adjust
    int dow = tm.tm_wday - 1;
    if (dow == -1)
        dow = 6;

    return tt_schedule_filter_runson_daysofweek(s, 1 << dow);
}
