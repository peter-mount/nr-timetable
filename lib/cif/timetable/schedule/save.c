#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/cif.h>
#include <area51/file.h>
#include <area51/hashmap.h>
#include <area51/log.h>
#include <area51/string.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>

/*
 * Writes the schedule .sch file
 * 
 */

static bool writeSchedule(void *k, void *v, void *c) {
    fwrite(v, sizeof (struct Schedule), 1, (FILE *) c);

    return true;
}

int tt_schedule_write(char *filename) {
    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->schedules, writeSchedule, f);
    fclose(f);
    return EXIT_SUCCESS;
}

static bool writeScheduleEntry(void *k, void *v, void *c) {
    FILE *f = (FILE *) c;

    struct Schedule *s = hashmapGet(timetable->scheduleSID, k);
    if (s) {
        int n = s->numEntries;
        fwrite(k, sizeof (unsigned int), 1, f);
        fwrite(&n, sizeof (int), 1, f);
        fwrite(v, sizeof (struct ScheduleEntry), n, f);
    } else {
        int *sid = k;
        logconsole("No schedule %ld", *sid);
        //abort();
    }

    return true;
}

int tt_schedule_write_entries(char *filename) {

    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->scheduleEntry, writeScheduleEntry, f);
    fclose(f);
    return EXIT_SUCCESS;
}
