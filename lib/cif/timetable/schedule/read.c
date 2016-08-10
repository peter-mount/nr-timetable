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
    // Read the schedule to get the number of entries
    struct Schedule temp;
    fread(&temp, sizeof (struct Schedule), 1, f);

    // Now allocate the true entry of the right size and insert into the map
    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule) + (temp.numEntries * sizeof (struct ScheduleEntry)));
    if (s) {
        memcpy(s, &temp, sizeof (struct Schedule));

        struct Schedule *e = &s[1];
        fread(e, sizeof (struct ScheduleEntry), temp.numEntries, f);
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
