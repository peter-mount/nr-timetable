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

static char *genFilename(char *filename, char *suffix) {
    char *db = genurl(filename, suffix);
    logconsole("Writing %s", db);
    return db;
}

int timetable_save(struct TimeTable *tt, char *filename) {

    char *db = genFilename(filename, TT_SUFFIX_META);
    int ret = saveMeta(db);
    free(db);

    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_TIPLOC);
        ret = tt_tiploc_write(db);
        free(db);
    }

    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_SCHEDULES);
        ret = tt_schedule_write(db);
        free(db);
    }

    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_ENTRIES);
        ret = tt_schedule_write_entries(db);
        free(db);
    }

    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_INDEX);
        ret = tt_write_index_stanox(db);
        free(db);
    }

    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_UID_INDEX);
        ret = tt_schedule_lookup_write(db);
        free(db);
    }

/*
 * Disabled association persistence as it currently blows up
    if (!ret) {
        db = genFilename(filename, TT_SUFFIX_ASSOCIATIONS);
        ret = tt_schedule_write_associations(db);
        free(db);
    }
*/

    logconsole(ret ? "Failed to write database %s" : "Completed database %s", filename);
    return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}
