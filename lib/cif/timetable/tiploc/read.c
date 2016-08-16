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

static bool indexTiploc(void *k, void *v, void *c) {
    struct TTTiploc *t = (struct TTTiploc *) v;

    hashmapPut(timetable->idTiploc, &t->id, t);

    // Should be 1-1 linkage here
    if (t->crs[0]) {
        struct TTTiploc *e = (struct TTTiploc *) hashmapGet(timetable->crsTiploc, t->crs);
        if (e)
            logconsole("CRS %s on %s already linked to %s?", t->crs, t->tiploc, e->tiploc);
        else
            hashmapPut(timetable->crsTiploc, t->crs, t);
    }

    if (t->stanox > 0)
        add(timetable->stanoxTiploc, &t->stanox, t);

    return true;
}

int tt_tiploc_read(Hashmap *m, char *filename) {
    int fsock = open(filename, O_RDONLY);
    if (fsock == -1) {
        logconsole("Cannot open %s", filename);
        return EXIT_FAILURE;
    }

    hashmapReadMemMap(m, readTiploc, fsock);
    logconsole(TT_LOG_FORMAT_D, "Tiplocs", hashmapSize(m));

    hashmapForEach(timetable->loc, indexTiploc, NULL);
    logconsole(TT_LOG_FORMAT_D, "CRS/3alpha", hashmapSize(timetable->crsTiploc));
    logconsole(TT_LOG_FORMAT_D, "Stanox", hashmapSize(timetable->stanoxTiploc));

    return EXIT_SUCCESS;
}
