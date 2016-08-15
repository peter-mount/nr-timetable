
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
    {'X', "Does not run on specified Bank Holiday Mondays"},
    {'E', "Does not run on specified Endinburgh Holiday dates (no longer used)"},
    {'G', "Does not run on specified Glasgow Holiday dates"}
};

int ttref_parse_bankHoliday(char v) {
    if (v != ' ')
        for (int i = 0; i < SIZE; i++)
            if (data[i].v == v)
                return i + 1;
    return 0;
}

char ttref_print_bankHoliday(int id) {
    return id < 1 || id > SIZE ? ' ' : data[id - 1].v;
}
