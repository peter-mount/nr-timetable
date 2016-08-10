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

static void *readTiploc(Hashmap *m, void *p) {
    struct TTTiploc *t = (struct TTTiploc *) p;
    hashmapPut(m, t->tiploc, t);
    return t+1;
}

// NOTE: this may break if a tiploc is deleted or renamed
// during import. I'll probably write a non-memmapped version
// for import, keeping this for the main application

int tt_tiploc_read(Hashmap *m, char *filename) {
    logconsole("Reading tiplocs from %s", filename);

    int fsock = open(filename, O_RDONLY);
    if (fsock == -1) {
        logconsole("Cannot open %s", filename);
        return EXIT_FAILURE;
    }

    hashmapReadMemMap(m, readTiploc, fsock);
    
    logconsole("Read %d", hashmapSize(m));
    return EXIT_SUCCESS;
}
