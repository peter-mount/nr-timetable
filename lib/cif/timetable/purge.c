#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

struct ctx {
    struct TimeTable *t;
    time_t deadline;
    int removed;
};

void purgeExpiredSchedule(struct Schedule *s) {
    free(s);
}

static bool callback(void *key, void *value, void *context) {
    struct ctx *ctx = (struct ctx *) context;
    struct Schedule *s = (struct Schedule *) value;
    if (s->end < ctx->deadline) {
        hashmapRemove(ctx->t->schedules, key);
        purgeExpiredSchedule(s);
        ctx->removed++;
    }

    return true;
}

/**
 * Purges all expired schedules from the timetable
 * 
 * @param t
 */
void purgeExpiredSchedules(struct TimeTable *t) {
    struct ctx ctx;
    memset(&ctx, 0, sizeof (struct ctx));

    ctx.t = t;

    // Get the deadline of midnight (00:00) of current day)
    struct tm tm;
    time(&ctx.deadline);
    localtime_r(&ctx.deadline, &tm);
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    ctx.deadline = mktime(&tm);

    hashmapForEach(t->schedules, callback, &ctx);
    logconsole("Removed %d expired schedules", ctx.removed);
}
