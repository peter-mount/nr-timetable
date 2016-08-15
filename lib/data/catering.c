
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

#define SIZE 6

static const struct table data[] = {
    {'C', "Buffet Service"},
    {'F', "Restaurant Car available for First Class passengers"},
    {'H', "Service of hot food available"},
    {'M', "Meal included for First Class passengers"},
    {'R', "Restaurant"},
    {'T', "Trolley Service"}
};

int ttref_parse_catering(char *v) {
    int c = 0;
    for (int j = 0; j < 4; j++)
        if (v[j] != ' ')
            for (int i = 0; i < SIZE; i++)
                if (data[i].v == v[j]) {
                    c = c << 3;
                    c |= i;
                }
    return c;
}

void ttref_print_catering(struct charbuffer *b, int dr) {
    if (!dr)
        charbuffer_append(b, "null");
    else {
        bool first = true;
        int m = 3;
        for (int i = 0; i < 4; i++) {
            int v = dr&m;
            m = m << 3;
            if (v) {
                charbuffer_add(b, first ? '[' : ',');
                first=true;
                json_append_str(b, data[v].label);
            }
        }
        if (first)
            charbuffer_append(b, "null");
        else
            charbuffer_add(b, ']');
    }
}
