#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <area51/stream.h>
#include <networkrail/timetable.h>

struct ctx {
    int stanox;
    int hour;
};

static bool filter(void *d, void *c) {
    if (!d)
        return false;

    if (!c)
        return true;

    struct ctx *ctx = c;
    struct Schedule *s = d;
    struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->sid);

    // Run through all as a stanox can exist more than once
    if (entries)
        for (int i = 0; i < s->numEntries; i++) {
            short tiploc = entries[i].time.tiploc;
            struct TTTiploc *tpl = hashmapGet(timetable->idTiploc, &tiploc);
            if (tpl && tpl->stanox == ctx->stanox) {
                int t = (scheduleTime_getTime(&entries[i].time) / 3600) % 24;
                if (t == ctx->hour)
                    return true;
            }
        }

    return false;
}

/*
 * Filter a stream of struct Schedule to contain schedules that contain a
 * specific stanox and will be at that stanox on a specific hour
 * 
 * @param s Stream
 * @param t time_t to filter against
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
int tt_filter_schedule_contains_stanox_hour(Stream *s, time_t *t, int stanox) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (!ctx)
        return EXIT_FAILURE;

    struct tm tm;
    memset(&tm, 0, sizeof (struct tm));
    gmtime_r(t, &tm);

    ctx->stanox = stanox;
    ctx->hour = tm.tm_hour;

    int r = stream_filter(s, filter, ctx, free);
    if (r)
        free(ctx);
    return r;
}