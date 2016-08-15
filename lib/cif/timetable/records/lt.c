#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/lookupTable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

int tt_parse_loc_terminating(struct CIFParser *parser) {
    struct TimeTable *t = (struct TimeTable *) parser;
    struct Schedule *s = t->curschedule;
    if (!s)
        return EXIT_FAILURE;

    struct ScheduleEntry *e = tt_parse_nextScheduleEntry(t);
    memset(e, 0, sizeof (struct ScheduleEntry));

    // Origin
    e->type = 'T';

    int offset = 2;

    char tiploc[10];
    offset = cif_readString(parser->buf, offset, tiploc, 7);
    struct TTTiploc *tpl = tt_getTiploc(t, tiploc);
    s->dest = e->tiploc = tpl->id;

    // Tiploc suffix for circular routes
    e->tiplocseq = parser->buf[offset++];

    offset = cif_readSeconds_hhmmh_r(parser->buf, offset, &e->wta);

    offset = cif_readSeconds_hhmm_r(parser->buf, offset, &e->pta);

    e->platform = tt_idmap_add_r(parser->buf, offset, 3);
    offset += 3;
    
    e->path = tt_idmap_add_r(parser->buf, offset, 3);
    offset += 3;

    e->activity=ttref_parse_activity(&parser->buf[offset]);
    offset+=12;

    offset = cif_readString(parser->buf, offset, e->resThameslink, 3);
    if(e->resThameslink[0])
        logconsole("TL \"%s\"",e->resThameslink);

    return EXIT_SUCCESS;
}
