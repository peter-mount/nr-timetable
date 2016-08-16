/*
 * Manages the idmap/textmap Hashmaps in Timetable used to normalise common
 * strings within the schedule like platform numbers and other values to keep
 * the schedule table size down.
 */
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/log.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>

extern struct TimeTable *timetable;

static void add(char *k, int id) {
    char *kc = strdup(k);
    if (!kc)
        return;

    int *v = (int *) malloc(sizeof (long));
    if (v) {
        *v = id;
        hashmapPut(timetable->idmap, kc, v);
        hashmapPut(timetable->txtmap, v, kc);
    } else
        free(kc);
}

int tt_idmap_add(char *k) {
    if (!k || k[0] == 0)
        return 0;

    int *v = (int *) hashmapGet(timetable->idmap, k);
    if (v)
        return *v;

    int id = (int) hashmapSize(timetable->idmap) + 1;
    add(k, id);
    return id;
}

int tt_idmap_add_r(char *k, int o, int l) {
    char s[l + 1];
    memset(s, 0, l + 1);
    cif_readString(k, o, s, l);
    return tt_idmap_add(s);
}

char *tt_idmap_get(int k) {
    if (k < 1)
        return NULL;

    return (char *) hashmapGet(timetable->txtmap, &k);
}

static bool writeText(void *k, void *v, void *c) {
    FILE *f = (FILE *) c;

    char text[16];
    memset(text, 0, 16);
    strncpy(text, (char *) k, 15);

    fwrite(v, sizeof (int), 1, f);
    fwrite(text, 16, 1, f);
    return true;
}

void tt_idmap_write(FILE *f) {
    hashmapWrite(timetable->idmap, writeText, f);
    logconsole("Written %d constants", (int) hashmapSize(timetable->idmap));
}

static bool readText(Hashmap *m, FILE *f) {
    int id;
    char text[16];
    fread(&id, sizeof (id), 1, f);
    fread(text, 16, 1, f);
    add(text, id);
    return true;
}

void tt_idmap_read(FILE *f) {
    hashmapRead(timetable->idmap, readText, f);
    logconsole(TT_LOG_FORMAT_D, "Constants", (int) hashmapSize(timetable->idmap));
}