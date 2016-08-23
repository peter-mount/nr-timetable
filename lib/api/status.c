#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/lookupTable.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <area51/list.h>
#include <area51/log.h>
#include <area51/stream.h>

#include "status-int.h"

static void addHeader(CharBuffer *b) {
    struct TTHeader *hdr = &timetable->header;

    charbuffer_append(b, "\"cif\":{");

    charbuffer_append(b, "\"extractTime\":");
    json_append_datetime_ISO(b, &hdr->extractTime);

    charbuffer_append(b, ",\"userStart\":");
    json_append_datetime_ISO(b, &hdr->userStart);

    charbuffer_append(b, ",\"userEnd\":");
    json_append_datetime_ISO(b, &hdr->userEnd);

    charbuffer_append(b, ",\"mainframeId\":");
    json_append_str(b, hdr->mainframeId);

    charbuffer_append(b, ",\"fileRef\":");
    json_append_str(b, hdr->fileRef);

    charbuffer_append(b, ",\"lastFileRef\":");
    json_append_str(b, hdr->lastFileRef);

    charbuffer_add(b, '}');
}

static const char *version = "\"version\":{"
        "\"version\":" PKGVER
        ",\"release\":" PKGREL
        ",\"date\":\"" PKGDATE
        "\",\"build\":\"" PKGBUILD
        "\"}";

static void addVersion(CharBuffer *b) {
    charbuffer_append(b, (char *) version);
}

static void addStats(CharBuffer *b) {
    charbuffer_append(b, "\"stats\":{");

    struct TTHeader *hdr = &timetable->header;
    
    charbuffer_append(b, "\"importTime\":");
    json_append_datetime_ISO(b, &hdr->importTime);

    charbuffer_append(b, ",\"tiplocs\":");
    json_append_int(b, hashmapSize(timetable->loc));

    charbuffer_append(b, ",\"crs\":");
    json_append_int(b, hashmapSize(timetable->crsTiploc));

    charbuffer_append(b, ",\"associations\":");
    json_append_int(b, hashmapSize(timetable->assoc));

    charbuffer_append(b, ",\"schedules\":");
    json_append_int(b, hashmapSize(timetable->schedules));

    charbuffer_append(b, ",\"uids\":");
    json_append_int(b, hashmapSize(timetable->uid));

    charbuffer_append(b, ",\"constants\":");
    json_append_int(b, hashmapSize(timetable->idmap));

    charbuffer_add(b, '}');
}

/*
 * Displays status and other details about the currently loaded timetable
 */
void tt_api_status(CharBuffer *b) {

    charbuffer_add(b, '{');

    addHeader(b);

    charbuffer_add(b, ',');
    addStats(b);

    charbuffer_add(b, ',');
    addVersion(b);

    charbuffer_add(b, '}');
}