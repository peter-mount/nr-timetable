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

static bool read(Hashmap *m, FILE *f) {
    // Now allocate the true entry of the right size and insert into the map
    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule));
    if (s) {
        memset(s,0,sizeof(struct Schedule));
        fread(s, sizeof (struct Schedule), 1, f);

        if (s->numEntries > 0) {
            s->entries = (struct ScheduleEntry *) malloc(s->numEntries * sizeof (struct ScheduleEntry));
            if (!s->entries) {
                free(s);
                return false;
            } else
                fread(s->entries, sizeof (struct ScheduleEntry), s->numEntries, f);
        } else
            s->entries = NULL;

        hashmapPut(m, &s->id, s);
        return true;
    }

    return false;
}

int tt_schedule_load(Hashmap *m, char *filename) {
    logconsole("Reading schedules from %s", filename);

    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(m, read, f);
    fclose(f);

    logconsole("Read %d schedules", hashmapSize(m));
    return EXIT_SUCCESS;
}
