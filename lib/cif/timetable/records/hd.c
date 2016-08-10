#include <stdio.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <stdlib.h>
#include <stdbool.h>

static bool callback(struct Node *node, void *context) {
    return true;
}

int tt_parse_header(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    cif_readString(parser->buf, 2, p->header.mainframeId, TT_HEADER_MAINFRAMEID);

    struct tm tm;
    p->header.importTime = cif_readDate_ddmmyyhhmm(parser->buf, 22, &tm);

    cif_readString(parser->buf, 32, p->header.fileRef, TT_HEADER_FILE_REF);
    cif_readString(parser->buf, 39, p->header.lastFileRef, TT_HEADER_FILE_REF);

    int update = parser->buf[46] == 'U';

    p->header.userStart = cif_readDate_ddmmyy(parser->buf, 48, &tm);

    p->header.userEnd = cif_readDate_ddmmyy(parser->buf, 54, &tm);

    char tm1[32], tm2[32], tm3[32];
    localtime_r(&p->header.importTime, &tm);
    strftime(tm1, 30, "%Y-%m-%d %H:%M:%S", &tm);

    localtime_r(&p->header.userStart, &tm);
    strftime(tm2, 30, "%Y-%m-%d", &tm);

    localtime_r(&p->header.userEnd, &tm);
    strftime(tm3, 30, "%Y-%m-%d", &tm);

    logconsole("Importing %s (%s) runs from %s to %s as %s",
            p->header.fileRef,
            tm1,
            tm2,
            tm3,
            update ? "Update" : "Full Import");

    return EXIT_SUCCESS;
}
