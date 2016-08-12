
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <string.h>
#include <ctype.h>

#include <area51/list.h>
#include <area51/log.h>

extern struct TimeTable *timetable;

void tt_append_scheduleEntry(struct charbuffer *b, struct ScheduleEntry *e) {
    if (e) {
        charbuffer_append(b, "{\"type\":\"");
        // TODO e->type=='R" then use C not L here
        charbuffer_add(b, 'L');
        charbuffer_add(b, e->type);

        charbuffer_append(b, "\",\"tiploc\":");
        json_append_int(b, (int) e->tiploc);
        //json_append_str(b, e->tiploc);

        charbuffer_append(b, ",\"tiplocSeq\":\"");
        charbuffer_add(b, e->tiplocseq);
        charbuffer_add(b, '"');

        append_hhmm(b, "pta", e->pta);
        append_hhmm(b, "ptd", e->ptd);

        append_hhmmss(b, "wta", e->wta);
        append_hhmmss(b, "wtd", e->wtd);
        append_hhmmss(b, "wtp", e->wtp);
        
        charbuffer_add(b, '}');
    } else {
        charbuffer_append(b, "{}");
    }
}
