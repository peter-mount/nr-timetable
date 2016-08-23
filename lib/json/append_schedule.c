
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <area51/list.h>
#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>

void tt_append_schedule(CharBuffer *b, struct Schedule *e) {
    if (e) {
        struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &e->sid);

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

        if (entries) {
            charbuffer_add(b, ',');
            tt_append_tiploc_field(b, "origin", entries[0].time.tiploc);
            charbuffer_add(b, ',');
            tt_append_tiploc_field(b, "dest", entries[e->numEntries - 1].time.tiploc);
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

        if (e->headcode) {
            charbuffer_printf(b, ",\"headcode\":\"%04d\"", e->headcode);
        }

        if (e->trainId) {
            charbuffer_append(b, ",\"trainId\":");
            json_append_str(b, tt_idmap_get(e->trainId));
        }

        if (e->serviceCode) {
            charbuffer_append(b, ",\"serviceCode\":");
            json_append_int(b, e->serviceCode);
        }

        if (e->timingLoad) {
            charbuffer_append(b, ",\"timingLoad\":");
            json_append_str(b, tt_idmap_get(e->timingLoad));
            //json_append_str(b, e->timingLoad);
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
        if (entries) {
            charbuffer_append(b, ",\"entries\":");
            charbuffer_add(b, '[');
            for (int i = 0; i < e->numEntries; i++) {
                if (i > 0)
                    charbuffer_add(b, ',');
                tt_append_scheduleEntry(b, &entries[i]);
            }
            charbuffer_add(b, ']');
        } else
            charbuffer_append(b, ",\"entries\":[]");

        charbuffer_add(b, '}');
    } else {

        charbuffer_append(b, "{}");
    }
}

void tt_append_schedule_node(CharBuffer *b, Node *n) {
    tt_append_schedule(b, (struct Schedule *) n->name);
}
