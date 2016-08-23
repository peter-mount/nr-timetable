
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>

/*
 * Append a ScheduleTime to json
 */
void tt_append_scheduleTime(CharBuffer *b, struct ScheduleTime *t) {
    tt_append_tiploc_field(b, "tiploc", t->tiploc);

    if (t->pta) append_hhmm(b, "pta", t->pta);
    if (t->ptd) append_hhmm(b, "ptd", t->ptd);

    if (t->wta) append_hhmmss(b, "wta", t->wta);
    if (t->wtd) append_hhmmss(b, "wtd", t->wtd);
    if (t->wtp) append_hhmmss(b, "wtp", t->wtp);

    if (t->platform) {
        charbuffer_append(b, ",\"platform\":");
        json_append_str(b, tt_idmap_get(t->platform));
    }

    if (t->activity) {
        charbuffer_append(b, ",\"activity\":");
        ttref_print_activity(b, t->activity);
    }
}
