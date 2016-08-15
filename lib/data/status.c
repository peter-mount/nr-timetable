
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>
#include <stdbool.h>

struct table {
    char c;
    char *t;
    unsigned int permanent : 1;
    unsigned int wtt : 1;
    unsigned int defined : 1;
    unsigned int : 5;
};

#define bus "Bus"
#define freight "Freight"
#define pax "Passenger & Parcels"
#define ship "Ship"
#define trip "Trip"

#define SIZE 10

static const struct table data[] = {
    {'B', bus, true, false, true},
    {'F', freight, true, true, true},
    {'P', pax, true, true, true},
    {'S', ship, true, false, true},
    {'T', trip, true, false, true},
    {'1', pax, true, true, true},
    {'2', freight, true, true, true},
    {'3', trip, true, false, true},
    {'4', ship, true, false, true},
    {'5', bus, true, false, true}
};

char ttref_print_status(int id) {
    return id < 1 || id > SIZE ? ' ' : data[id - 1].c;
}

int ttref_parse_status(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].c == v)
                return i + 1;
    return 0;
}
