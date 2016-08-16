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

static bool indexSchedule(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *) v;

    // schedule uuid
    add(timetable->uid, s->id.uid, s);

    return true;
}

static bool indexSchedule2(void *k, void *v, void *c) {
    struct Schedule *s = (struct Schedule *) v;

    int *max = (int *) c;

    if (*max < s->numEntries)
        *max = s->numEntries;

    return true;
}

void tt_index() {
    logconsole("Indexing tiplocs");
    hashmapForEach(timetable->loc, indexTiploc, NULL);
    logconsole(TT_LOG_FORMAT_D, "crs index", hashmapSize(timetable->crsTiploc));
    logconsole(TT_LOG_FORMAT_D, "stanox index", hashmapSize(timetable->stanoxTiploc));

    logconsole("Indexing schedules");
    hashmapForEach(timetable->schedules, indexSchedule, NULL);
    logconsole(TT_LOG_FORMAT_D, "uid index", hashmapSize(timetable->uid));

    int max = 0;
    hashmapForEach(timetable->schedules, indexSchedule2, &max);
    logconsole("Max entries in schedule %d", max);
}
