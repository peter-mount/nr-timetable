
#include <stdio.h>
#include <stdlib.h>
#include <networkrail/timetable/lookupTable.h>
#include <limits.h>

struct table {
    char v;
    char *label;
};

#define SIZE 10

static const struct table data[] = {
    {'Z', "Operator-specific useage"},
    {'0', "Portion 0 (NRS))"},
    {'1', "Portion 1 (NRS))"},
    {'2', "Portion 2 (NRS))"},
    {'3', "Portion 3 (NRS)"},
    {'4', "Portion 4 (NRS)"},
    {'5', "Portion 5 (NRS))"},
    {'6', "Portion 6 (NRS))"},
    {'7', "Portion 7 (NRS))"},
    {'8', "Portion 8 (NRS)"}
};

int ttref_parse_portionId(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].v == v)
                return i + 1;
    return 0;
}

char ttref_print_portionId(int id) {
    return id < 1 || id > SIZE ? ' ' : data[id - 1].v;
}
