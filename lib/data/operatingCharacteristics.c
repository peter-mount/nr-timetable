
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <area51/charbuffer.h>
#include <area51/json.h>
#include <area51/rest.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char v;
    char *label;
};

#define SIZE 12

static const struct table data[] = {
    {'B', "Vacuum Braked"},
    {'C', "Timed at 100mph"},
    {'D', "Driver Only Operation (Coaching stock trains)"},
    {'E', "Conveys Mk4 Coaches"},
    {'G', "Trainman (Guard) required"},
    {'M', "Timed at 110mph"},
    {'P', "Push/Pull train"},
    {'Q', "Runs as required"},
    {'R', "Air conditioned with PA system"},
    {'S', "Steam heated"},
    {'Y', "Runs to Terminals/Yards as required"},
    {'Z', "May convey traffic to SB1C guage. Not to be diverted from booked route without authority"},
};

int ttref_parse_opchar(char *v) {
    int c = 0;
    for (int j = 0; j < 4; j++)
        if (v[j] != ' ')
            for (int i = 0; i < SIZE; i++)
                if (data[i].v == v[j]) {
                    c = c << 4;
                    c |= i;
                }
    return c;
}

void ttref_print_opchar(struct charbuffer *b, int dr) {
    if (!dr)
        charbuffer_append(b, "null");
    else {
        bool first = true;
        int m = 7;
        for (int i = 0; i < 6; i++) {
            int v = dr&m;
            m = m << 4; 
            if (v) {
                charbuffer_add(b, first ? '[' : ',');
                first = true;
                json_append_str(b, data[v].label);
            }
        }
        if (first)
            charbuffer_append(b, "null");
        else
            charbuffer_add(b, ']');
    }
}
