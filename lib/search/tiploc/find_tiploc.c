
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

void tt_find_tiploc(struct charbuffer *b, const char *tiploc) {
    char *s = strdup(tiploc);
    for (char *p = s; *p; p++)
        *p = toupper(*p);

    struct TTTiploc *tpl = (struct TTTiploc *) hashmapGet(timetable->loc, s);
    tt_append_tiploc(b,tpl);
}