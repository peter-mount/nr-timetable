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

// Callback to read in a schedule

static bool read(Hashmap *m, FILE *f) {
    // Now allocate the true entry of the right size and insert into the map
    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule));
    if (s) {
        memset(s, 0, sizeof (struct Schedule));
        fread(s, sizeof (struct Schedule), 1, f);

        if (s->numEntries > 0) {
            s->entries = (struct ScheduleEntry *) malloc(s->numEntries * sizeof (struct ScheduleEntry));
            if (!s->entries) {
                free(s);
                return false;
            } else
                fread(s->entries, sizeof (struct ScheduleEntry), s->numEntries, f);
        } else
            s->entries = NULL;

        hashmapPut(m, &s->id, s);
        return true;
    }

    return false;
}

// Util to add an index entry where the entry is a linked list

static void add(Hashmap *m, void *k, void *e) {
    struct List *l = (struct List *) hashmapGet(m, k);
    if (!l) {
        l = (struct List *) malloc(sizeof (struct List));
        list_init(l);
        hashmapPut(m, k, l);
    }

    struct Node *n = node_alloc((char *) e);
    list_addTail(l, n);
}

static bool indexSchedule(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *) v;

    // schedule uuid
    add(timetable->uid, s->id.uid, s);

    return true;
}

int tt_schedule_load(Hashmap *m, char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(m, read, f);
    fclose(f);

    logconsole(TT_LOG_FORMAT_D, "Schedules", hashmapSize(m));

    hashmapForEach(timetable->schedules, indexSchedule, NULL);
    logconsole(TT_LOG_FORMAT_D, "UID", hashmapSize(timetable->uid));

    return EXIT_SUCCESS;
}
