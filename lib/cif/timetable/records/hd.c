#include <stdio.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <stdlib.h>
#include <stdbool.h>

extern struct TimeTable *timetable;

static bool callback(Node *node, void *context) {
    return true;
}

void tt_log_header(char *task) {
    struct tm tm;
    char tm1[32], tm2[32], tm3[32];
    localtime_r(&timetable->header.extractTime, &tm);
    strftime(tm1, 30, "%Y-%m-%d %H:%M:%S", &tm);

    localtime_r(&timetable->header.userStart, &tm);
    strftime(tm2, 30, "%Y-%m-%d", &tm);

    localtime_r(&timetable->header.userEnd, &tm);
    strftime(tm3, 30, "%Y-%m-%d", &tm);

    logconsole(TT_LOG_FORMAT_P "%s exported %s", task, timetable->header.fileRef, tm1);

    logconsole(TT_LOG_FORMAT_P "%s to %s", "Period", tm2, tm3);
}

int tt_parse_header(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    cif_readString(parser->buf, 2, p->header.mainframeId, TT_HEADER_MAINFRAMEID);

    struct tm tm;
    p->header.extractTime = cif_readDate_ddmmyyhhmm(parser->buf, 22, &tm);

    cif_readString(parser->buf, 32, p->header.fileRef, TT_HEADER_FILE_REF);
    cif_readString(parser->buf, 39, p->header.lastFileRef, TT_HEADER_FILE_REF);

    int update = parser->buf[46] == 'U';

    p->header.userStart = cif_readDate_ddmmyy(parser->buf, 48, &tm);

    p->header.userEnd = cif_readDate_ddmmyy(parser->buf, 54, &tm);

    tt_log_header(update ? "Update extract" : "Full extract");

    return EXIT_SUCCESS;
}
