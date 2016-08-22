#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/hashmap.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/string.h>
#include <area51/comparator.h>

// Sorts each schedule list in order of start date and stpIndicator

static char *STP = "CNOP";

static int comparator(Node *a, Node *b) {

    struct Schedule *sa = (struct Schedule *) a->name;
    struct Schedule *sb = (struct Schedule *) b->name;

    // Sort by start
    int r = comparator_time_t(&sa->id.start, &sb->id.start);

    // When equal sort by end
    if (!r)
        r = comparator_time_t(&sa->end, &sb->end);

    // If equal sort by STP
    if (!r)
        r = comparator_long(
            (long*) strchrnul(STP, sa->id.stpInd),
            (long *) strchrnul(STP, sb->id.stpInd)
            );

    return r;
}

static bool sortSchedules(void *k, void *v, void *c) {
    List *l = v;
    list_sort(l, comparator);
    return true;
}

// Callback to read in a schedule

static bool read(Hashmap *m, FILE *f) {
    // Now allocate the true entry of the right size and insert into the map
    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule));
    if (s) {
        memset(s, 0, sizeof (struct Schedule));
        fread(s, sizeof (struct Schedule), 1, f);
        hashmapPut(m, &s->id, s);

        if (s->numEntries > 0) {
            struct ScheduleEntry *entries = (struct ScheduleEntry *) malloc(s->numEntries * sizeof (struct ScheduleEntry));
            if (!entries) {
                free(s);
                return false;
            }

            fread(entries, sizeof (struct ScheduleEntry), s->numEntries, f);
            hashmapPut(timetable->scheduleEntry, &s->id, entries);
        }
        
        return true;
    }

    return false;
}

static bool mapUid(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *) v;

    hashmapAddList(timetable->uid, s->id.uid, s);

    return true;
}

void tt_schedule_load_uid() {
    hashmapForEach(timetable->schedules, mapUid, NULL);
    logconsole(TT_LOG_FORMAT_D, "UID", hashmapSize(timetable->uid));

    logconsole("Sorting...");
    hashmapForEach(timetable->uid, sortSchedules, NULL);
}

int tt_schedule_load(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(timetable->schedules, read, f);
    fclose(f);

    logconsole(TT_LOG_FORMAT_D, "Schedules", hashmapSize(timetable->schedules));

    tt_schedule_load_uid();

    return EXIT_SUCCESS;
}
