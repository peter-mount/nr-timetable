#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <area51/cif.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>

void timetable_initParser(struct TimeTable *tt) {
    cif_initParser(&tt->parser);
    // Define the handlers, order doesn't matter here
    Hashmap *handlers = tt->parser.handlers;
    hashmapPut(handlers, "HD", tt_parse_header);

    hashmapPut(handlers, "TD", tt_parse_ttdelete);
    hashmapPut(handlers, "TI", tt_parse_ttinsert);
    hashmapPut(handlers, "TU", tt_parse_ttinsert);

    hashmapPut(handlers, "AA", tt_parse_assoc);

    hashmapPut(handlers, "BS", tt_parse_basic_schedule);
    hashmapPut(handlers, "LO", tt_parse_loc_origin);
    hashmapPut(handlers, "LI", tt_parse_loc_intermediate);
    hashmapPut(handlers, "LT", tt_parse_loc_terminating);

    hashmapPut(handlers, "ZZ", tt_parse_trailer);
}
