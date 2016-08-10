#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

int tt_parse_scheduleId(struct CIFParser *parser, struct ScheduleId *id) {
    struct tm tm;
    cif_readString(parser->buf, 3, id->uid, TT_UID_LENGTH);
    id->start = cif_readDate_yymmdd(parser->buf, 3 + TT_UID_LENGTH, &tm);
//    id->end = cif_readDate_yymmdd(parser->buf, 3 + TT_UID_LENGTH+4, &tm);
    id->stpInd = parser->buf[79];
    return EXIT_SUCCESS;
}
