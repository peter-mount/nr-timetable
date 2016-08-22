
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
#define SIZE 35

// Chars 0 & 1 are the code in CIF
// Chars 2... is the text. If none (i.e. we don't know it) then chars 0/1 are used
static const char *data[] = {
    "A ", "Stops or shunts for other trains to pass",
    "AE", "Attach/detach assisting locomotive",
    "AX", "Shows as 'X' on arrival",
    "BL", "Stops for banking locomotive",
    "C ", "Stops to change trainmen",
    "D ", "Stops to set down passengers",
    "-D", "Stops to detach vehicles",
    "E ", "Stops for examination",
    "G ", "National Rail Timetable data to add",
    "H ", "Notional activity to prevent WTT timing columns merge",
    "HH", "As H where a third column is involved",
    "K ", "Passenger count point",
    "KC", "Ticket collection & examination point",
    "KE", "Ticket examination point",
    "KF", "Ticket examination point, 1st Class only",
    "KS", "Selective ticket examination point",
    "L ", "Stops to change locomotives",
    "N ", "Stop not advertised",
    "OP", "Stops for other operating reasons",
    "OR", "Train locomotive on rear",
    "PR", "Propelling between points shown",
    "R ", "Stops when required",
    "RM", "Reversing movement, or driver changes ends",
    "RR", "Stops for locomotive to run around train",
    "S ", "Stops for railway personnel only",
    "T ", "Stops to take up & set down passengers",
    "-T", "Stops to attach & detach vehicles",
    "TB", "Train begins (Origin)",
    "TF", "Train finishes (Destination)",
    "TS", "Details Consist for TOPS Direct",
    "TW", "Stops (or pass) for tablet, staff or token",
    "U ", "Stops to take up passengers",
    "-U", "Stops to attach vehicles",
    "W ", "Stops to watering of coaches",
    "X ", "Passes another train at crossing point on single line"
};

long ttref_parse_activity(char *v) {
    long c = 0;
    char *s = v;
    for (int j = 0; j < 6; j++, s += 2)
        if (*s != ' ') {
            bool q = false;
            for (int i = 0, k = 0; !q && i < SIZE; i++, k += 2)
                if (data[k][0] == s[0] && data[k][1] == s[1]) {
                    c = c << 6;
                    c |= i;
                    q = true;
                }
            if (!q)
                logconsole("Unsupported activity '%c%c'", s[0], s[1]);
        }
    return c;
}

void ttref_print_activity(CharBuffer *b, long dr) {
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
                json_append_str(b, (char *) data[v << 1]);
            }
        }
        if (first)
            charbuffer_add(b, ']');
        else
            charbuffer_append(b, "null");
    }
}

void ttref_add_activity(Hashmap *map, long dr) {
    if (dr) {
        long m = 63;
        for (int i = 0; i < 6; i++) {
            long v = dr&m;
            m = m << 6;
            if (v) {
                v = v << 1;
                if (!hashmapContainsKey(map, (void *) data[v]))
                    hashmapPut(map, (void *) data[v], (void *) data[v + 1]);
            }
        }
    }
}

void ttref_add_schedule_activity(Hashmap *map, struct Schedule *s) {
    struct ScheduleEntry *entries = hashmapGet(timetable->scheduleEntry, &s->id);
    if (entries)
        for (int i = 0; i < s->numEntries; i++)
            ttref_add_activity(map, entries[i].activity);
}

struct ctx {
    CharBuffer *b;
    bool sep;
};

static bool append(void *k, void *v, void *c) {
    struct ctx *ctx = (struct ctx *) c;
    charbuffer_printf(ctx->b, ctx->sep ? ",\"%s\":\"%s\"" : "\"%s\":\"%s\"", k, v);
    ctx->sep = true;
    return true;
}

void ttref_print_activity_index(CharBuffer *b, Hashmap *m) {
    struct ctx ctx;
    ctx.b = b;
    ctx.sep = false;
    hashmapForEach(m, append, &ctx);
}