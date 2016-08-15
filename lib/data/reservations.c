
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char v;
    char *label;
    char *symbol;
};

#define SIZE 4

static const struct table data[] = {
    {'A', "Seat Reservations Compulsory", "R_white_box"},
    {'E', "Reservations for Bicycles Essential", "triangle_inverted_black"},
    {'R', "Seat Reservations Recommended", "R_black_box"},
    {'S', "Seat Reservations possible from any station", "diamond_white"}
};

int ttref_parse_reservations(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].v == v)
                return i + 1;
    return 0;
}

char *ttref_print_reservations(int id) {
    return id < 1 || id > SIZE ? NULL : data[id - 1].label;
}
