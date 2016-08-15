#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <area51/charbuffer.h>
#include <networkrail/timetable/lookupTable.h>

struct table {
    char c;
    char *t;
    unsigned int permanent : 1;
    unsigned int wtt : 1;
    unsigned int : 6;
};

#define bus "Bus"
#define freight "Freight"
#define pax "Passenger & Parcels"
#define ship "Ship"
#define trip "Trip"

#define SIZE 10

static struct table data[] = {
    {'B', bus, true, false},
    {'F', freight, true, true},
    {'P', pax, true, true},
    {'S', ship, true, false},
    {'T', trip, true, false},
    {'1', pax, false, false},
    {'2', freight, false, false},
    {'3', trip, false, false},
    {'4', ship, false, false},
    {'5', bus, false, false}
};

void ttref_print_status(struct charbuffer *b, int id) {
    if (id < 1 || id > SIZE) {
        charbuffer_append(b, "null");
        return;
    }

    struct table *t = &data[id - 1];
    
    charbuffer_add(b,'"');

    if (!t->permanent)
        charbuffer_append(b, "STP ");
    
    charbuffer_append(b, t->t);

    if (t->permanent)
        charbuffer_append(b, t->wtt ? " (Permanent - WTT)" : " (Permanent)");
    
    charbuffer_add(b,'"');
}

int ttref_parse_status(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].c == v)
                return i + 1;
    return 0;
}
