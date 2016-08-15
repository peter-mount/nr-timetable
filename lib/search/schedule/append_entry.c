
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>

#include <area51/list.h>
#include <area51/log.h>

static char type[] = {
    'O', 'I', 'T'
};

void tt_append_scheduleEntry(struct charbuffer *b, struct ScheduleEntry *e) {
    if (e) {
        charbuffer_append(b, "{\"type\":\"");
        // TODO e->type=='R" then use C not L here
        charbuffer_add(b, 'L');
        charbuffer_add(b, type[e->type]);
        charbuffer_add(b, '\"');

        charbuffer_add(b, ',');
        tt_append_tiploc_field(b, "tiploc", e->tiploc);

        if (e->tiplocseq && e->tiplocseq != ' ') {
            charbuffer_append(b, ",\"tiplocSeq\":\"");
            charbuffer_add(b, e->tiplocseq);
            charbuffer_add(b, '"');
        }

        if (e->pta) append_hhmm(b, "pta", e->pta);
        if (e->ptd) append_hhmm(b, "ptd", e->ptd);

        if (e->wta) append_hhmmss(b, "wta", e->wta);
        if (e->wtd) append_hhmmss(b, "wtd", e->wtd);
        if (e->wtp) append_hhmmss(b, "wtp", e->wtp);

        if (e->platform) {
            charbuffer_append(b, ",\"platform\":");
            json_append_str(b, tt_idmap_get(e->platform));
        }

        if (e->line) {
            charbuffer_append(b, ",\"line\":");
            json_append_str(b, tt_idmap_get(e->line));
        }

        if (e->path) {
            charbuffer_append(b, ",\"path\":");
            json_append_str(b, tt_idmap_get(e->path));
        }

        if (e->activity) {
            charbuffer_append(b, ",\"activity\":");
            ttref_print_activity(b, e->activity);
        }

        if (e->engAllow) {
            charbuffer_append(b, ",\"engAllow\":");
            json_append_str(b, tt_idmap_get(e->engAllow));
        }

        if (e->pathAllow) {
            charbuffer_append(b, ",\"pathAllow\":");
            json_append_str(b, tt_idmap_get(e->pathAllow));
        }

        if (e->perfAllow) {
            charbuffer_append(b, ",\"perfAllow\":");
            json_append_str(b, tt_idmap_get(e->perfAllow));
        }

        charbuffer_add(b, '}');
    } else {
        charbuffer_append(b, "{}");
    }
}
