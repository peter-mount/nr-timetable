#include <stdio.h>

#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/hashmap.h>
#include <area51/cif.h>

int tt_parse_trailer(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    // Complete any previous schedule
    tt_parse_complete_schedule(p);

    logconsole(TT_LOG_FORMAT_D, "Lines read", parser->lineno);
    logconsole(TT_LOG_FORMAT_D, "Bytes read", parser->fileLength);

    logconsole(TT_LOG_FORMAT_D, "Constants", (int) hashmapSize(timetable->idmap));
    logconsole(TT_LOG_FORMAT_D, "Tiplocs", hashmapSize(p->loc));
    logconsole(TT_LOG_FORMAT_D, "Associations", hashmapSize(p->assoc));
    logconsole(TT_LOG_FORMAT_D, "Schedules", hashmapSize(p->schedules));

    return EXIT_SUCCESS;
}
