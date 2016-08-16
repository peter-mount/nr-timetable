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

extern struct TimeTable *timetable;

static int saveMeta(char *db) {
    logconsole("Writing metadata to %s", db);
    backupFile(db);
    FILE *f = fopen(db, "w");
    if (!f) return EXIT_FAILURE;

    fwrite(&timetable->header, sizeof (struct TTHeader), 1, f);
    tt_idmap_write(f);
    fclose(f);
    return EXIT_SUCCESS;
}

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

int timetable_save(struct TimeTable *tt, char *filename) {
    logconsole("Writing timetable %s", filename);

    char *db = genurl(filename, ".db");
    int ret = saveMeta(db);
    free(db);

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
