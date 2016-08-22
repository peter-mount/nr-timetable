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
#include <fcntl.h>

// callback to read a tiploc entry
static void *readTiploc(Hashmap *m, void *p) {
    struct TTTiploc *t = (struct TTTiploc *) p;
    hashmapPut(m, t->tiploc, t);
    return t + 1;
}

static bool indexTiploc(void *k, void *v, void *c) {
    struct TTTiploc *t = (struct TTTiploc *) v;

    // tiploc id lookup
    hashmapPut(timetable->idTiploc, &t->id, t);

    // Should be 1-1 linkage here
    if (t->crs[0]) {
        struct TTTiploc *e = (struct TTTiploc *) hashmapGet(timetable->crsTiploc, t->crs);
        if (e)
            logconsole("CRS %s on %s already linked to %s?", t->crs, t->tiploc, e->tiploc);
        else
            hashmapPut(timetable->crsTiploc, t->crs, t);
    }

    // stanox can reference multiple tiplocs
    if (t->stanox > 0)
        hashmapAddList(timetable->stanoxTiploc, &t->stanox, t);

    return true;
}

void tt_tiploc_index() {
    logconsole(TT_LOG_FORMAT_D, "Tiplocs", hashmapSize(timetable->loc));

    hashmapForEach(timetable->loc, indexTiploc, NULL);
    logconsole(TT_LOG_FORMAT_D, "CRS/3alpha", hashmapSize(timetable->crsTiploc));
    logconsole(TT_LOG_FORMAT_D, "Stanox", hashmapSize(timetable->stanoxTiploc));
}

int tt_tiploc_read(char *filename) {
    int fsock = open(filename, O_RDONLY);
    if (fsock == -1) {
        logconsole("Cannot open %s", filename);
        return EXIT_FAILURE;
    }

    hashmapReadMemMap(timetable->idTiploc, readTiploc, fsock);
    tt_tiploc_index();

    return EXIT_SUCCESS;
}
