
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

        charbuffer_append(b, ",\"stpInd\":\"");
        charbuffer_add(b, e->id.stpInd);
        charbuffer_add(b, '"');

        // ---

        charbuffer_append(b, ",\"end\":");
        json_append_date_ISO(b, &e->end);

        if (e->origin) {
            charbuffer_add(b, ',');
            tt_append_tiploc_field(b, "origin", e->origin);
        }

        if (e->dest) {
            charbuffer_add(b, ',');
            tt_append_tiploc_field(b, "dest", e->dest);
        }

        // daysRun
        charbuffer_append(b, ",\"daysRun\":");
        ttref_print_daysRun(b, e->daysRun);

        if (e->bankHolRun) {
            charbuffer_append(b, ",\"bankHolRun\":\"");
            json_append_str(b, ttref_print_bankHoliday(e->bankHolRun));
            charbuffer_add(b, '"');
        }

        if (e->status) {
            charbuffer_append(b, ",\"status\":");
            ttref_print_status(b, e->status);
        }

        if (e->category) {
            charbuffer_append(b, ",\"category\":");
            json_append_str(b, ttref_print_trainCategory(e->category));
        }

        if (e->headcode[0]) {
            charbuffer_append(b, ",\"headcode\":");
            json_append_str(b, e->headcode);
        }

        if (e->trainId[0]) {
            charbuffer_append(b, ",\"trainId\":");
            json_append_str(b, e->trainId);
        }

        if (e->serviceCode) {
            charbuffer_append(b, ",\"serviceCode\":");
            json_append_int(b, e->serviceCode);
        }

        if (e->timingLoad[0]) {
            charbuffer_append(b, ",\"timingLoad\":");
            json_append_str(b, e->timingLoad);
        }

        if (e->speed) {
            charbuffer_append(b, ",\"speed\":");
            json_append_int(b, e->speed);
        }

        if (e->powerType) {
            charbuffer_append(b, ",\"powerType\":");
            json_append_str(b, ttref_print_powerType(e->powerType));
        }

        if (e->portionId) {
            charbuffer_append(b, ",\"portionId\":\"");
            charbuffer_add(b, ttref_print_portionId(e->portionId));
            charbuffer_add(b, '"');
        }

        if (e->operatingCharacteristics) {
            charbuffer_append(b, ",\"operatingCharacteristics\":");
            ttref_print_opchar(b, e->operatingCharacteristics);
        }

        // Schedule entries
        if (e->numEntries) {
            charbuffer_append(b, ",\"entries\":");
            charbuffer_add(b, '[');
            for (int i = 0; i < e->numEntries; i++) {
                if (i > 0)
                    charbuffer_add(b, ',');
                tt_append_scheduleEntry(b, &e->entries[i]);
            }
            charbuffer_add(b, ']');
        } else
            charbuffer_append(b, ",\"entries\":[]");

        charbuffer_add(b, '}');
    } else {

        charbuffer_append(b, "{}");
    }
}

void tt_append_schedule_node(struct charbuffer *b, struct Node *n) {
    tt_append_schedule(b, (struct Schedule *) n->name);
}
