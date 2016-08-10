#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

struct TTTiploc *tt_getTiploc(struct TimeTable *t, char *tiploc) {
    struct TTTiploc *tpl = (struct TTTiploc *) hashmapGet(t->loc, tiploc);

    // Should never occur but if it does means bug in CIF files or missing data
    if (!tpl) {
        logconsole("Missing tiploc %s, creating dummy entry but db is inconsistent", tiploc);
        tpl = (struct TTTiploc *) malloc(sizeof (struct TTTiploc));
        if (tpl) {
            memset(tpl, 0, sizeof (struct TTTiploc));
            strncpy(tpl->tiploc, tiploc, 7);
            tpl->id = t->locSeq++;
            hashmapPut(t->loc, tpl->tiploc, tpl);
        }
    }
    
    return tpl;
}
