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
    struct Schedule *s = (struct Schedule *) v;
    FILE *f = (FILE *) c;

    fwrite(s, sizeof (struct Schedule), 1, f);

    if (s->numEntries > 0) {
        struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->id);
        if (entries == NULL) {
            logconsole("Schedule without entries");
            abort();
        }
        fwrite(entries, sizeof (struct ScheduleEntry), s->numEntries, f);
    }

    return true;
}

int tt_schedule_write(char *filename) {

    logconsole("Writing schedule to %s", filename);

    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->schedules, writeSchedule, f);
    fclose(f);
    return EXIT_SUCCESS;
}
