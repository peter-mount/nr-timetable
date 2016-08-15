
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <area51/charbuffer.h>
#include <area51/log.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

// If this hits 64 then we must resize the fields in ScheduleEntry
#define SIZE 36

// Chars 0 & 1 are the code in CIF
// Chars 2... is the text. If none (i.e. we don't know it) then chars 0/1 are used
static const char *data[] = {
    "A Stops or shunts for other trains to pass",
    "AEAttach/detach assisting locomotive",
    "AXShows as 'X' on arrival",
    "BLStops for banking locomotive",
    "C Stops to change trainmen",
    "D Stops to set down passengers",
    "-DStops to detach vehicles",
    "E Stops for examination",
    "G National Rail Timetable data to add",
    "H Notional activity to prevent WTT timing columns merge",
    "HHAs H where a third column is involved",
    "K Passenger count point",
    "KCTicket collection & examination point",
    "KETicket examination point",
    "KFTicket examination point, 1st Class only",
    "KSSelective ticket examination point",
    "L Stops to change locomotives",
    "N Stop not advertised",
    "OPStops for other operating reasons",
    "ORTrain locomotive on rear",
    "PRPropelling between points shown",
    "R Stops when required",
    "RMReversing movement, or driver changes ends",
    "RRStops for locomotive to run around train",
    "S Stops for railway personnel only",
    "T Stops to take up & set down passengers",
    "-TStops to attach & detach vehicles",
    "TBTrain begins (Origin)",
    "TCUnknown activity 'TC'",
    "TFTrain finishes (Destination)",
    "TSDetails Consist for TOPS Direct",
    "TWStops (or pass) for tablet, staff or token",
    "U Stops to take up passengers",
    "-UStops to attach vehicles",
    "W Stops to watering of coaches",
    "X Passes another train at crossing point on single line"
};

long ttref_parse_activity(char *v) {
    long c = 0;
    char *s = v;
    for (int j = 0; j < 6; j++, s += 2)
        if (*s != ' ') {
            bool q = false;
            for (int i = 0; !q && i < SIZE; i++)
                if (data[i][0] == s[0] && data[i][1] == s[1]) {
                    c = c << 6;
                    c |= i;
                    q = true;
                }
            if (!q)
                logconsole("Unsupported activity '%c%c'", s[0], s[1]);
        }
    return c;
}

void ttref_print_activity(struct charbuffer *b, long dr) {
    if (!dr)
        charbuffer_append(b, "null");
    else {
        bool first = true;
        long m = 63;
        for (int i = 0; i < 6; i++) {
            long v = dr&m;
            m = m << 6;
            if (v) {
                charbuffer_add(b, first ? '[' : ',');
                first = true;
                char *s = (char *) &data[v][2];
                if (!s)
                    s = (char *) data[v];
                json_append_str(b, s);
            }
        }
        if (first)
            charbuffer_add(b, ']');
        else
            charbuffer_append(b, "null");
    }
}
