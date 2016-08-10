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

static bool readTiploc(Hashmap *m, FILE *f) {
    struct TTTiploc *l = (struct TTTiploc *) malloc(sizeof (struct TTTiploc));
    if (l) {
        fread(l, sizeof (struct TTTiploc), 1, f);
        hashmapPut(m, l->tiploc, l);
        return true;
    }
    return false;
}

// Set locSeq to be next free value

static bool huntNextLocSeq(void *k, void *v, void *c) {
    struct TimeTable *t = (struct TimeTable *) c;
    struct TTTiploc *loc = (struct TTTiploc *) v;
    short seq = loc->id + 1;
    if (t->locSeq < seq)
        t->locSeq = seq;
    return true;
}

int timetable_load(struct TimeTable *tt, char *filename) {
    logconsole("Reading timetable %s", filename);

    int ret = EXIT_SUCCESS;
    char *db = genurl(filename, ".db");
    FILE *f = fopen(db, "r");
    free(db);
    if (!f) ret = EXIT_FAILURE;
    else {
        fread(&tt->header, sizeof (struct TTHeader), 1, f);
        fclose(f);
    }

    if (!ret) {
        // See note for tt_tiploc_read - mmap mode may break the importer?
        db = genurl(filename, ".loc");
        ret = tt_tiploc_read(tt->loc, db);
        //f = fopen(db, "r");
        free(db);
        //if (!f) ret = EXIT_FAILURE;
        //else {
        //    hashmapRead(tt->loc, readTiploc, f);
        
        // Ensure tt->locSeq is the true next version if we create a new tiploc
        if (!ret) {
            tt->locSeq = 0;
            logconsole("Scanning");
            hashmapForEach(tt->loc, huntNextLocSeq, tt);
            //fclose(f);
            logconsole("Next tiploc idSeq %d", (int) tt->locSeq);
        }
    }
    
    if(!ret) {
        db = genurl(filename,".sch");
        ret = tt_schedule_load(tt->schedules,db);
        free(db);
    }

    logconsole(ret ? "Failed to Read database %s" : "Loaded database %s", filename);
    return ret ? EXIT_FAILURE : EXIT_SUCCESS;
}
