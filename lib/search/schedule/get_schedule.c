
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>

#include <area51/list.h>
#include <area51/log.h>

extern struct TimeTable *timetable;

// https://uktra.in/timetable/schedule/W46666/2016-09-12

extern int hashmapShortHash(void* key);
extern bool hashmapShortEquals(void* keyA, void* keyB);

/*
 * Returns all schedules for a specific UID
 * 
 * example URI: /schedule/uid/W46666
 * 
 * returns a json object:
 * {
 *   "schedule": [],
 *   "tiploc": {}
 * }
 * 
 * where schedule is an array of the schedules
 * tiploc is an object containing the tiploc's in the schedules, used for
 * display purposes
 * 
 */
void tt_get_schedules_by_uid(struct charbuffer *b, const char *uid) {
    char *s = strdup(uid);
    for (char *p = s; *p; p++)
        *p = toupper(*p);

    // Simply use our list of schedule id's
    struct List * l = (struct List *) hashmapGet(timetable->uid, s);
    free(s);

    tt_schedules_result(b, l, NULL, NULL);

}