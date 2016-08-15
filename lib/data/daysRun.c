
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <area51/charbuffer.h>
#include <networkrail/timetable/lookupTable.h>

static char *dow = "MonTueWedThuFriSatSun";

int ttref_parse_daysRun(char *s) {
    int d = 0;
    for (int i = 0; i < 7; i++)
        if (s[i] == '1')
            d |= 1 << i;
    return d;
}

static void append(struct charbuffer *b, int s, int e) {
    if (s >= 0 && s < e) {
        charbuffer_put(b, &dow[s * 3], 3);
        // Successive days separate with , otherwise range -
        charbuffer_add(b, (s+1)==e?',':'-');
    }
    charbuffer_put(b, &dow[e * 3], 3);
}

void ttref_print_daysRun(struct charbuffer *b, int dr) {
    bool notFirst = false;
    int prev = INT_MIN, start = INT_MIN;
    for (int i = 0; i < 7; i++) {
        int mask = 1 << i;
        bool set = dr & mask;

        if (set) {
            if (start < 0)
                start = i;
            prev = i;
        } else {
            if (prev >= 0) {
                charbuffer_add(b, notFirst ? ',' : '"');
                notFirst = true;
                append(b, start, prev);
            }
            start = prev = INT_MIN;
        }
    }

    if (prev >= 0) {
        charbuffer_add(b, notFirst ? ',' : '"');
        notFirst = true;
        append(b, start, prev);
    }

    if (notFirst)
        charbuffer_add(b, '"');
    else
        charbuffer_append(b, "NULL");
}
