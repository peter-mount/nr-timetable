#define _GNU_SOURCE

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
#include <area51/comparator.h>
#include <fcntl.h>

static void *readMemMap(Hashmap *m, void *p) {
    char *uid = p;
    p += TT_UID_LENGTH + 1;
    hashmapPut(m, uid, p);

    int *n = p;
    return p + sizeof (int)+(sizeof (struct Assoc)*(*n));
}

int tt_schedule_load_associations(char *filename) {

    int f = open(filename, O_RDONLY);
    if (f == -1)
        return EXIT_FAILURE;

    hashmapReadMemMap(timetable->assoc, readMemMap, f);

    logconsole(TT_LOG_FORMAT_D, "Associations", hashmapSize(timetable->assoc));

    return EXIT_SUCCESS;
}
