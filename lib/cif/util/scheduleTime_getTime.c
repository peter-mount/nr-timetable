#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable.h>

/*
 * Retrieves the time at the location from a ScheduleTime in seconds from midnight.
 * 
 * This is in the sequence of ptd, pta, wtd, wta, wtp in order of precedence.
 */
int scheduleTime_getTime(struct ScheduleTime *t) {
    // Note this works as there are no valid entries for Midnight (00:00:00)
    // within the timetable.
    int tm = t->ptd * 60;
    if (tm == 0) tm = t->pta * 60;
    if (tm == 0) tm = t->wtd;
    if (tm == 0) tm = t->wta;
    if (tm == 0) tm = t->wtp;
    return tm;
}
