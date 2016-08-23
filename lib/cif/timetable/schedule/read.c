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
#include <fcntl.h>

// If USE_MEMMAP is defined then schedules will be read in a memory mapped hashmap
// rather than reading the tables directly into memory
#define USE_MEMMAP 1

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

#ifdef USE_MEMMAP

static void *readMemMap(Hashmap *m, void *p) {
    struct Schedule *s = p;
    hashmapPut(m, &s->id, s);
    return s + 1;
}

#else

static bool read(Hashmap *m, FILE *f) {
    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule));
    if (s) {
        memset(s, 0, sizeof (struct Schedule));
        fread(s, sizeof (struct Schedule), 1, f);
        hashmapPut(m, &s->id, s);
        return true;
    }
    return false;
}
#endif

#ifdef USE_MEMMAP

static void *readEntryMemMap(Hashmap *m, void *p) {
    unsigned int *sid = p;
    int *n = sid + 1;
    int *ei = n + 1;
    struct ScheduleEntry *e = (struct ScheduleEntry *) ei;
    hashmapPut(m, sid, e);
    return e + *n;
}

#else

static bool readEntry(Hashmap *m, FILE *f) {
    // Now allocate the true entry of the right size and insert into the map
    unsigned int *s = malloc(sizeof (unsigned int));
    if (s) {
        fread(s, sizeof (unsigned int), 1, f);

        int n;
        fread(&n, sizeof (int), 1, f);

        struct ScheduleEntry *e = malloc(sizeof (struct ScheduleEntry)*n);
        if (!e) {
            free(s);
            return false;
        }

        fread(e, sizeof (struct ScheduleEntry), n, f);

        hashmapPut(m, s, e);

        return true;
    }

    return false;
}
#endif

static bool mapUid(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *) v;

    hashmapAddList(timetable->uid, s->id.uid, s);

    return true;
}

void tt_schedule_load_uid() {
    hashmapForEach(timetable->schedules, mapUid, NULL);
    logconsole(TT_LOG_FORMAT_D, "UID", hashmapSize(timetable->uid));

    logconsole("Sorting UID schedules...");
    hashmapForEach(timetable->uid, sortSchedules, NULL);
}

int tt_schedule_load(char *filename) {

#ifdef USE_MEMMAP
    int f = open(filename, O_RDONLY);
    if (f == -1)
        return EXIT_FAILURE;

    hashmapReadMemMap(timetable->schedules, readMemMap, f);
#else
    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(timetable->schedules, read, f);
    fclose(f);
#endif

    logconsole(TT_LOG_FORMAT_D, "Schedules", hashmapSize(timetable->schedules));

    tt_schedule_load_uid();

    return EXIT_SUCCESS;
}

int tt_schedule_load_entries(char *filename) {

#ifdef USE_MEMMAP
    int f = open(filename, O_RDONLY);
    if (f == -1)
        return EXIT_FAILURE;

    hashmapReadMemMap(timetable->scheduleEntry, readEntryMemMap, f);
#else
    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(timetable->scheduleEntry, readEntry, f);
    fclose(f);
#endif

    logconsole(TT_LOG_FORMAT_D, "Entries", hashmapSize(timetable->scheduleEntry));

    tt_schedule_load_uid();

    return EXIT_SUCCESS;
}
