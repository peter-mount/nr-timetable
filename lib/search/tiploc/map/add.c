
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

void mapTiploc_addTiploc(Hashmap *m, short tiploc) {
    if (tiploc > 0) {
        struct TTTiploc *tpl = (struct TTTiploc *) hashmapGet(m, &tiploc);
        if (!tpl) {
            tpl = (struct TTTiploc *) hashmapGet(timetable->idTiploc, &tiploc);

            if (tpl)
                hashmapPut(m, &tpl->id, tpl);
        }
    }
}
