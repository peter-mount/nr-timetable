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
 * Returns all schedules for a stanox location on a specific date and optionally hour of day.
 * 
 * We use Stanox rather than CRS or TIPLOC as a station can have more than one
 * CRX or TIPLOC but has just one stanox. For example London Victoria has
 * stanox 87201 but CRS code VIC (in public use) & also XVR. As for TIPLOC's it
 * has 17 (at the time of writing).
 * 
 * example URI: /schedule/stanox/{stanox}/{YY-MM-DD}
 * example URI: /schedule/stanox/{stanox}/{YY-MM-DD}T{HH}
 * 
 * Returns the schedule list json object:
 * {
 *   "stanox": stanox,
 *   "date": "YY-MM-DD",
 *   "hour": HH,
 *   "trains": [],
 *   "tiploc": {},
 *   "activity": {}
 * }
 * 
 * where schedule is an array of the schedules
 * tiploc is an object containing the tiploc's in the schedules,
 * activity is an object of the activities at each location in the schedules.
 * 
 * tiploc & activity are used for display purposes as they contain the full
 * text for each location etc.
 * 
 */

void tt_get_schedules_by_stanox(CharBuffer *b, const char *uri) {

    char tmp[strlen(uri) + 1];
    strcpy(tmp, uri);

    // Find first / separating the data - if not present then give up
    char *dateSpec = strchr(tmp, '/');
    if (dateSpec == NULL)
        return;
    *dateSpec++ = 0;

    // The stanox, must be >0
    int stanox = atoi(tmp);
    if (stanox < 1)
        return;

    // Extract the date
    struct tm tm;

    // Get midnight today as our base limit
    time_t now;
    time(&now);
    gmtime_r(&now, &tm);
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    now = mktime(&tm);

    // Parse the time
    memset(&tm, 0, sizeof (struct tm));
    bool hourPresent = strchr(dateSpec, 'T');
    strptime(dateSpec, hourPresent ? "%Y-%m-%dT%H" : "%Y-%m-%d", &tm);
    time_t t = mktime(&tm);

    // If before today then give up
    if (t < now)
        return;

    logconsole("Date %04d-%02d-%02dT%02d hourPresent %d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, hourPresent);

    // Get a Stream of struct Schedule that pass through this stanox
    Stream *stream = tt_search_schedules_by_stanox(stanox);
    if (!stream)
        return;

    int r = 0; //EXIT_FAILURE;

    if(!r) r = stream_notNull(stream);
    
    // Now collect using the schedule result collector and run
    if (!r) r = tt_schedule_result_full(stream, b);

    stream_debug(stream);
    if (!r)
        stream_run(stream, NULL);
    else
        stream_free(stream);

}
