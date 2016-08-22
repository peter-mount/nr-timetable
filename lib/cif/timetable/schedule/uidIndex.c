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
#include <area51/file.h>

struct entry {
    int id;
    struct ScheduleId sid;
};

static bool writeUID(void *k, void *v, void *c) {
    struct index *index = v;
    FILE *f = c;

    fwrite(k, sizeof (int), 1, f);
    fwrite(v, sizeof (struct ScheduleId), 1, f);

    return true;
}

int tt_schedule_lookup_write(char *filename) {
    logconsole("Writing scheduleId index %s", filename);

    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->schedId, writeUID, f);
    fclose(f);
    return EXIT_SUCCESS;
}

static bool read(Hashmap *m, FILE *f) {
    struct entry *e = malloc(sizeof (struct entry));
    if (!e)
        return false;

    fread(e, sizeof (struct entry), 1, f);
    hashmapPut(m, &e->id, &e->sid);
    return true;
}

int tt_schedule_lookup_load(char *filename) {
    logconsole(TT_LOG_FORMAT_S, "Reading", filename);

    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(timetable->schedId, read, f);
    fclose(f);

    logconsole(TT_LOG_FORMAT_D, "Stanox-Schedule", hashmapSize(timetable->schedId));

    return EXIT_SUCCESS;
}
