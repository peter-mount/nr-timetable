
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
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

    struct List * l = (struct List *) hashmapGet(timetable->uid, s);

    charbuffer_append(b, "{\"schedule\":");
    json_append_list(b, l, tt_append_schedule_node);
    charbuffer_append(b, ",\"tiploc\":{");

    if (l && !list_isEmpty(l)) {
        // Generate index of tiplocs within the schedules
        Hashmap *m = mapTiploc_new();

        struct Node *n = list_getHead(l);
        while (list_isNode(n)) {
            mapTiploc_mapSchedule(m, (struct Schedule *) n->name);
            n = list_getNext(n);
        }

        mapTiploc_appendIndex(b, m);

        hashmapFree(m);
    }

    charbuffer_append(b, "}}");
}