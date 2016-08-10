#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/string.h>
#include <area51/file.h>

/*
 * 
 * Creates the database files
 * 
 * .db is the main timetable database file. It's small as it contains the
 * main metadata for the database only.
 * 
 * .loc is the contents of the tiploc hashMap. Separate so we can reuse it
 * separately in the future if we need to.
 * 
 * .sch is the schedule data.
 * 
 * 
 */

static bool writeSchedule(void *k, void *v, void *c) {
    FILE *f = (FILE *) c;
    if (fwrite(v, sizeof (struct Schedule), 1, f) == sizeof (struct Schedule))
        return false;

    struct Schedule *s = (struct Schedule *) v;
    int size = sizeof (struct Schedule) + (s->numEntries * sizeof (struct ScheduleEntry));
    fwrite(&size, sizeof (int), 1, f);
    fwrite(s, size, 1, f);

    return true;
}

int timetable_save(struct TimeTable *tt, char *filename) {
    logconsole("Writing timetable %s", filename);

    int ret = EXIT_SUCCESS;
    char *db = genurl(filename, ".db");
    logconsole("Writing metadata to %s", db);
    backupFile(db);
    FILE *f = fopen(db, "w");
    free(db);
    if (!f) ret = EXIT_FAILURE;
    else {
        fwrite(&tt->header, sizeof (struct TTHeader), 1, f);
        fclose(f);
    }

    // Save tiploc data
    if (!ret) {
        db = genurl(filename, ".loc");
        ret = tt_tiploc_write(tt->loc, db);
        free(db);
    }

    if (!ret) {
        db = genurl(filename, ".sch");
        logconsole("Writing schedule to %s", db);
        ret = tt_schedule_write(tt->schedules, db);
        free(db);
    }

    logconsole(ret ? "Failed to write database %s" : "Completed database %s", filename);
    return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}
