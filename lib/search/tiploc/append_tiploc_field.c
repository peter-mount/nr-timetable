
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

/**
 * Convenience method, appends a tiploc name field.
 * 
 * e.g. tt_append_tiploc_field(b,"origin",1234);
 * may append "origin":"MSTONEE" if id 1234 maps to that tiploc.
 * 
 * Note: If tiploc is 0 or there is no mapping then this will generate "origin":null
 */
void tt_append_tiploc_field(CharBuffer *b, char *n, short tiploc) {
    json_append_str(b, n);
    charbuffer_add(b, ':');

    struct TTTiploc *tpl = NULL;

    if (tiploc > 0)
        tpl = (struct TTTiploc *) hashmapGet(timetable->idTiploc, &tiploc);

    if (tpl)
        json_append_str(b, tpl->tiploc);
    else
        charbuffer_append(b, "null");
}
