#include <stdio.h>
#include <stdlib.h>
#include <area51/json.h>
#include <area51/log.h>
#include <area51/hashmap.h>
#include <networkrail/timetable.h>

#include <string.h>
#include <stdbool.h>

extern struct TimeTable *timetable;

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

static bool indexSchedule(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *)v;
    
    // schedule uuid
    add(timetable->uid,s->id.uid,s);
    
    return true;
}

void tt_index() {
    logconsole("Indexing tiplocs");
    hashmapForEach(timetable->loc, indexTiploc, NULL);
    
    logconsole("Indexing schedules");
    hashmapForEach(timetable->schedules, indexSchedule, NULL);
    logconsole("Schedules %d uids %d", hashmapSize(timetable->schedules),hashmapSize(timetable->uid));
}
