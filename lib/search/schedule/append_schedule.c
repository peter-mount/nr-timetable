
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

void tt_append_schedule(struct charbuffer *b, struct Schedule *e) {
    if (e) {
        charbuffer_append(b, "{\"uid\":");
        json_append_str(b, e->id.uid);

        charbuffer_append(b, ",\"start\":");
        // date from time_t e->id.start
        charbuffer_append(b, "null");

        charbuffer_append(b, ",\"stdInd\":\"");
        charbuffer_add(b, e->id.stpInd);
        charbuffer_add(b, '"');

        // ---

        // Schedule entries
        charbuffer_append(b, ",\"entries\":");
        charbuffer_add(b, '[');
        /*
                for (int i = 0; i < e->numEntries; i++) {
                    if (i > 0)
                        charbuffer_add(b, ',');
                    tt_append_scheduleEntry(b, &e->entries[i]);
                }
         */
        charbuffer_add(b, ']');

        charbuffer_add(b, '}');
    } else {

        charbuffer_append(b, "{}");
    }
}

void tt_append_schedule_node(struct charbuffer *b, struct Node *n) {
    tt_append_schedule(b, (struct Schedule *) n->name);
}
