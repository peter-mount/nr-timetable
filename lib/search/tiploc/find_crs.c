
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

void tt_find_tiploc_by_crs(struct charbuffer *b, const char *crs) {
    struct TTTiploc *tpl = NULL;

    if (crs && strlen(crs) == 3) {
        char *s = strdup(crs);
        for (char *p = s; *p; p++)
            *p = toupper(*p);

        tpl = (struct TTTiploc *) hashmapGet(timetable->crsTiploc, s);
    }

    tt_append_tiploc(b, tpl);
}