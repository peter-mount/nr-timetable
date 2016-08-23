#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

void tt_parse_ensure_capacity(struct TimeTable *t) {
    if (t->entryCount >= t->numEntries) {
        int oldSize = t->numEntries * sizeof (struct ScheduleEntry);
        t->numEntries += 10;
        
        int newSize = t->numEntries * sizeof (struct ScheduleEntry);
        struct ScheduleEntry *newEntries = (struct ScheduleEntry *) malloc(newSize);
        if (t->entries) {
            memcpy(newEntries, t->entries, oldSize);
            free(t->entries);
        }
        t->entries = newEntries;
    }
}

void tt_parse_complete_schedule(struct TimeTable *t) {
    if (t->curschedule) {
        t->curschedule->numEntries = t->entryCount;
        if (t->entryCount > 0) {
            unsigned int *sid = malloc(sizeof(unsigned int));
            *sid = t->curschedule->sid;
            
            int s = sizeof (struct ScheduleEntry)*t->entryCount;
            struct ScheduleEntry *entries = malloc(s);
            memcpy(entries, t->entries, s);
            
            hashmapPut(timetable->scheduleEntry,sid,entries);
        }
        t->entryCount = 0;
    }
}

struct ScheduleEntry *tt_parse_nextScheduleEntry(struct TimeTable *t) {
    tt_parse_ensure_capacity(t);
    t->entryCount++;
    return &t->entries[t->entryCount - 1];
}
