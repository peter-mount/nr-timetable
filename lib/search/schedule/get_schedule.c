
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

void tt_get_schedules_by_uid(struct charbuffer *b, const char *uid) {
    char *s = strdup(uid);
    for (char *p = s; *p; p++)
        *p = toupper(*p);

    struct List *l = (struct List *) hashmapGet(timetable->uid, s);
    json_append_list(b, l, tt_append_schedule_node);
}