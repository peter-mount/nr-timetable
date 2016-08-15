
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char v;
    char *label;
};

#define SIZE 3

static const struct table data[] = {
    {'F', "First class only"},
    {'S', "Standard class only"},
    {'B', "First & Standard class"},
};

int ttref_parse_sleepers(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].v == v)
                return i + 1;
    return 0;
}

char *ttref_print_sleepers(int id) {
    return id < 1 || id > SIZE ? NULL : data[id - 1].label;
}
