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

    logconsole("Read %d lines, %d bytes", parser->lineno, parser->fileLength);

    logconsole("      Tiplocs %d", hashmapSize(p->loc));
    logconsole(" Associations %d", hashmapSize(p->assoc));
    logconsole("    Schedules %d", hashmapSize(p->schedules));

    return EXIT_SUCCESS;
}
