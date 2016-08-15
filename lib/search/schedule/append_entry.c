
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

void tt_append_scheduleEntry(struct charbuffer *b, struct ScheduleEntry *e) {
    if (e) {
        charbuffer_append(b, "{\"type\":\"");
        // TODO e->type=='R" then use C not L here
        charbuffer_add(b, 'L');
        charbuffer_add(b, e->type);

        tt_append_tiploc_field(b,"tiploc",e->tiploc);

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

        if (e->platform[0]) {
            charbuffer_append(b, ",\"platform\":");
            json_append_str(b, e->platform);
        }

        if (e->line[0]) {
            charbuffer_append(b, ",\"line\":");
            json_append_str(b, e->line);
        }

        if (e->path[0]) {
            charbuffer_append(b, ",\"path\":");
            json_append_str(b, e->path);
        }

        // activity
        char ac = 0;
        for (int i = 0; i < 6; i++) {
            if (e->activity[i][0]) {
                if (ac)
                    charbuffer_add(b, ac);
                else {
                    charbuffer_append(b, ",\"activity\":[");
                    ac = ',';
                }
                json_append_str(b, e->activity[i]);
            }
        }
        if (ac)
            charbuffer_add(b, ']');

        if (e->engAllow[0]) {
            charbuffer_append(b, ",\"engAllow\":");
            json_append_str(b, e->engAllow);
        }

        if (e->pathAllow[0]) {
            charbuffer_append(b, ",\"pathAllow\":");
            json_append_str(b, e->pathAllow);
        }

        if (e->perfAllow[0]) {
            charbuffer_append(b, ",\"perfAllow\":");
            json_append_str(b, e->perfAllow);
        }

        if (e->resThameslink[0]) {
            charbuffer_append(b, ",\"resThameslink\":");
            json_append_str(b, e->resThameslink);
        }

        charbuffer_add(b, '}');
    } else {
        charbuffer_append(b, "{}");
    }
}
