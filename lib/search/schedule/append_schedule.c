
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

void json_append_date_ISO(struct charbuffer *b, time_t *t) {
    struct tm tm;
    if (t && *t) {
        gmtime_r(t, &tm);
        charbuffer_printf(b, "\"%04d-%02d-%02d\"", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday);
    } else
        json_append_str(b, "null");
}

void json_append_time_hhmm(struct charbuffer *b, time_t *t) {
    struct tm tm;
    if (t && *t) {
        gmtime_r(t, &tm);
        charbuffer_printf(b, "\"%02d:%02d\"", tm.tm_hour, tm.tm_min);
    } else
        json_append_str(b, "null");
}

void json_append_time_hhmmss(struct charbuffer *b, time_t *t) {
    struct tm tm;
    if (t && *t) {
        gmtime_r(t, &tm);
        charbuffer_printf(b, "\"%02d:%02d:%02d\"", tm.tm_hour, tm.tm_min, tm.tm_sec);
    } else
        json_append_str(b, "null");
}

void json_append_datetime_ISO(struct charbuffer *b, time_t *t) {
    struct tm tm;
    if (t && *t) {
        gmtime_r(t, &tm);
        charbuffer_printf(b, "\"%04d-%02d-%02dT%02d:%02d:%02dZ\"",
                1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
    } else
        json_append_str(b, "null");
}

void tt_append_schedule(struct charbuffer *b, struct Schedule *e) {
    if (e) {
        charbuffer_append(b, "{\"uid\":");
        json_append_str(b, e->id.uid);

        charbuffer_append(b, ",\"start\":");
        json_append_date_ISO(b, &e->id.start);

        charbuffer_append(b, ",\"stdInd\":\"");
        charbuffer_add(b, e->id.stpInd);
        charbuffer_add(b, '"');

        // ---

        charbuffer_append(b, ",\"end\":");
        json_append_date_ISO(b, &e->end);

        tt_append_tiploc_field(b,"origin",e->origin);
        tt_append_tiploc_field(b,"dest",e->dest);
        
        // daysRun
        charbuffer_append(b, ",\"daysRunV\":[]");

        charbuffer_append(b, ",\"daysRun\":\"");
        if (e->daysRun)
            charbuffer_add(b, e->daysRun);
        charbuffer_add(b, '"');

        charbuffer_append(b, ",\"bankHolRun\":\"");
        if (e->bankHolRun)
            charbuffer_add(b, e->bankHolRun);
        charbuffer_add(b, '"');

        charbuffer_append(b, ",\"status\":");
        json_append_str(b, e->status);

        charbuffer_append(b, ",\"category\":");
        json_append_str(b, e->category);

        charbuffer_append(b, ",\"headcode\":");
        json_append_str(b, e->headcode);

        charbuffer_append(b, ",\"serviceCode\":");
        json_append_str(b, e->serviceCode);

        charbuffer_append(b, ",\"portionId\":");
        json_append_str(b, e->portionId);

        // Schedule entries
        charbuffer_append(b, ",\"entries\":");
        charbuffer_add(b, '[');
        for (int i = 0; i < e->numEntries; i++) {
            if (i > 0)
                charbuffer_add(b, ',');
            tt_append_scheduleEntry(b, &e->entries[i]);
        }
        charbuffer_add(b, ']');

        charbuffer_add(b, '}');
    } else {

        charbuffer_append(b, "{}");
    }
}

void tt_append_schedule_node(struct charbuffer *b, struct Node *n) {
    tt_append_schedule(b, (struct Schedule *) n->name);
}
