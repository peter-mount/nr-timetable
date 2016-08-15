
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char *k;
    char *label;
};

#define SIZE 8

static const struct table data[] = {
    {"D  ", "Diesel"},
    {"DEM", "Diesel Electric Multiple Unit"},
    {"DMU", "Diesel Mechanical Multiple Unit"},
    {"E  ", "Electric"},
    {"ED ", "Electro-Diesel"},
    {"EML", "EMU + E, E, ED locomotive"},
    {"EMU", "Electric Multiple Unit"},
    {"HST", "High Speed Train"}
};

int ttref_parse_powerType(char a, char b, char c) {
    if (!(a == b == c == ' '))
        for (int i = 0; i < SIZE; i++)
            if (data[i].k[0] == a && data[i].k[1] == b && data[i].k[2] == c)
                return i + 1;
    return 0;
}

char *ttref_print_powerType(int id) {
    return id < 1 || id > SIZE ? NULL : data[id - 1].k;
}
