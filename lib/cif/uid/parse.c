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
    memset(id, 0, sizeof (struct ScheduleId));
    int o = 3;
    o = cif_readString(parser->buf, o, id->uid, TT_UID_LENGTH);
    o = cif_readDate_yymmdd_r(parser->buf, o, &tm, &id->start);
    id->stpInd = parser->buf[79];
    return EXIT_SUCCESS;
}
