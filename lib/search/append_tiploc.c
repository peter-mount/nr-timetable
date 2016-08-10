
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

void tt_append_tiploc(struct charbuffer *b, struct TTTiploc *tpl) {
    if (tpl) {
        charbuffer_append(b, "{\"id\":");
        json_append_int(b, tpl->id);

        charbuffer_append(b, ",\"tiploc\":");
        json_append_str(b, tpl->tiploc);

        charbuffer_append(b, ",\"crs\":");
        json_append_str(b, tpl->crs);

        charbuffer_append(b, ",\"stanox\":");
        json_append_int(b, tpl->stanox);

        charbuffer_append(b, ",\"nlc\":");
        json_append_int(b, tpl->nlc);

        charbuffer_append(b, ",\"nlc_check\":\"");
        charbuffer_add(b, tpl->nlc_check);
        charbuffer_add(b, '\"');

        charbuffer_append(b, ",\"desc\":");
        json_append_str(b, tpl->desc);

        charbuffer_append(b, ",\"nlcdesc\":");
        json_append_str(b, tpl->nlcdesc);

        charbuffer_add(b, '}');
    } else {
        charbuffer_append(b, "{}");
    }
}

void tt_append_tiploc_node(struct charbuffer *b, struct Node *n) {
    tt_append_tiploc(b, (struct TTTiploc *) n->name);
}

