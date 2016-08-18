
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>
#include <area51/list.h>

extern struct TimeTable *timetable;

void tt_find_tiploc_by_stanox(CharBuffer *b, int stanox) {
    List *l = (List *) hashmapGet(timetable->stanoxTiploc, &stanox);
    json_append_list(b,l,tt_append_tiploc_node);
}