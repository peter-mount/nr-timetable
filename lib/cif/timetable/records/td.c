#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/hashmap.h>
#include <area51/cif.h>
#include <area51/log.h>

int tt_parse_ttdelete(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    char tiploc[8];
    cif_readString(parser->buf, 2, tiploc, 7);
    struct TTTiploc *tpl = (struct TTTiploc *) hashmapRemove(p->loc, tiploc);
    if (tpl) {
        free(tpl);
    } else {
        logconsole("WARNING: Tiploc '%s' to delete but not found", tiploc);
    }

    return EXIT_SUCCESS;
}
