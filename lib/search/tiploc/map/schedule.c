
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>

void mapTiploc_mapScheduleEntry(Hashmap *m, struct ScheduleEntry *e) {
    mapTiploc_addTiploc(m, e->time.tiploc);
}

/**
 * Map all tiplocs in a schedule into the given map.
 * 
 * The map must be keyed against a short (i.e. TTTiploc.id)
 */
void mapTiploc_mapSchedule(Hashmap *m, struct Schedule *s) {
    struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->sid);
    if (entries)
        for (int i = 0; i < s->numEntries; i++)
            mapTiploc_mapScheduleEntry(m, &entries[i]);
}
