#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/cif.h>
#include <area51/file.h>
#include <area51/hashmap.h>
#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/string.h>

static bool writeTiploc(void *k, void *v, void *c) {
    return fwrite(v, sizeof (struct TTTiploc), 1, (FILE *) c) == sizeof (struct TTTiploc) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int tt_tiploc_write(char *filename) {

    logconsole("Writing tiplocs to %s", filename);

    backupFile(filename);
    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->loc, writeTiploc, f);
    fclose(f);
    return EXIT_SUCCESS;
}
