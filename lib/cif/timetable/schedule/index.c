/*
 * Creates the stanox -> schedule index hashmap which maps every schedule
 * that appears against every stanox location.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <area51/log.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/files.h>
#include <limits.h>
#include <area51/string.h>
#include <area51/file.h>

#include <string.h>

// Expanding array of ScheduleId's

#define CAPACITY 10

struct index {
    int stanox;
    int size;
    int capacity;
    int *id;
};

struct stat {
    int min;
    int max;
};

static int getId(struct ScheduleId *id) {
    int *sid = hashmapGet(timetable->idSched, id);

    if (!sid) {
        sid = malloc(sizeof (int));
        if (!sid) {
            logconsole("malloc failure");
            abort();
        }

        *sid = hashmapSize(timetable->idSched);
        hashmapPut(timetable->idSched, id->uid, sid);
        hashmapPut(timetable->schedId, sid, id->uid);
    }

    return *sid;
}

static void add(struct index *index, int sid) {
    if (index->size >= index->capacity) {
        int c = index->capacity + CAPACITY;

        int *n = malloc(sizeof (int)*c);
        if (n == NULL) {
            logconsole("Malloc stanox %d new capacity %d cur capacity %d", index->stanox, (int) index->capacity, (int) c);
            abort();
        }

        if (index->id) {
            memcpy(n, index->id, index->capacity * sizeof (int));
            free(index->id);
        }

        index->capacity = c;
        index->id = n;
    }

    index->id[index->size++] = sid;
}

static void indexEntry(int sid, struct ScheduleEntry *e, struct stat *stats) {
    int tiploc = e->tiploc;
    struct TTTiploc *t = hashmapGet(timetable->idTiploc, &tiploc);
    if (t && t->stanox > 0) {
        int stanox = t->stanox;
        struct index *index = hashmapGet(timetable->schedStanox, &stanox);
        if (!index) {
            index = malloc(sizeof (struct index));
            if (!index)
                return;

            index->stanox = stanox;
            index->size = 0;
            index->capacity = CAPACITY;
            index->id = malloc(sizeof (int)*index->capacity);

            hashmapPut(timetable->schedStanox, &index->stanox, index);
        }

        // Remove duplicates (e.g. circular schedules)
        for (int i = 0; i < index->size; i++)
            if (index->id[i] == sid)
                return;

        add(index, sid);

        // Update debug statistics
        if (index->size < stats->min)
            stats->min = index->size;

        if (index->size > stats->max)
            stats->max = index->size;
    }
}

static bool indexSchedule(void *k, void *v, void *c) {
    struct Schedule *s = v;

    int sid = getId(&s->id);

    for (int i = 0; i < s->numEntries; i++)
        indexEntry(sid, &s->entries[i], (struct stat *) c);

    return true;
}

int tt_schedule_index_stanox() {

    struct stat stats;
    stats.max = INT_MIN;
    stats.min = INT_MAX;

    hashmapForEach(timetable->schedules, indexSchedule, &stats);

    logconsole(TT_LOG_FORMAT_D, "Stanox-Schedule", hashmapSize(timetable->schedStanox));
    logconsole(TT_LOG_FORMAT_D, "min/stanox", stats.min);
    logconsole(TT_LOG_FORMAT_D, "max/stanox", stats.max);
}

static bool writeIndex(void *k, void *v, void *c) {
    struct index *index = v;
    FILE *f = c;
    
    // stanox & size int's
    fwrite(&index->stanox, sizeof (int), 2, f);

    // Index entries
    fwrite(index->id, sizeof (int), index->size, f);

    return true;
}

int tt_write_index_stanox(char *filename) {
    logconsole("Writing stanox-schedule index %s", filename);

    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->schedStanox, writeIndex, f);
    fclose(f);
    return EXIT_SUCCESS;
}
