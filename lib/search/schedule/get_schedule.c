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
 * Returns all schedules for a specific UID
 * 
 * example URI: /schedule/uid/W46666
 * example URI: /schedule/uid/W46666/YY-MM-DD
 * 
 * returns a json object:
 * {
 *   "schedule": [],
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
void tt_get_schedules_by_uid(struct charbuffer *b, const char *uri) {
    // writable copy of uri
    char uid[strlen(uri) + 1];
    strcpy(uid, uri);

    // Scan for / (optional date marker) making uid prefix upper case
    char *s = uid;
    for (; *s && *s != '/'; s++)
        *s = toupper(*s);

    // If / present then change to string terminator. s points to start of date
    bool datePresent = *s == '/';
    time_t t = 0;
    if (datePresent) {
        // Terminate uid & s points to start of date field
        *s++ = 0;

        // Extract the date
        struct tm tm;
        memset(&tm, 0, sizeof (struct tm));
        strptime(s, "%Y-%m-%d", &tm);
        t = mktime(&tm);
    }

    Stream *stream = list_stream((struct List *) hashmapGet(timetable->uid, uid));

    // Get Schedule from node->name
    list_map_node_name(stream);

    // Add dateFilter
    if (datePresent) {
        // Filter only schedules containing the specified date and find the first entry
        stream_filter(stream, tt_schedule_filter_date, &t, NULL);
        // Limit to days the schedules run against
        tt_schedule_filter_dayRunning(stream, &t);
        // We want just the first
        stream_findFirst(stream);
    }

    // Now collect using the schedule result collector and run
    tt_schedule_result(stream);
    //stream_debug(stream);
    stream_run(stream, b);

}
