
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

Hashmap *mapTiploc_new() {
    return hashmapCreate(100, hashmapShortHash, hashmapShortEquals);
}
