#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/cif.h>
#include <area51/log.h>

int tt_parse_assoc(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    char ttype = parser->buf[2];

    char muid[7], auid[7];
    cif_readString(parser->buf, 3, muid, 6);
    cif_readString(parser->buf, 9, auid, 6);

    // FIXME implement

    return EXIT_SUCCESS;
}
