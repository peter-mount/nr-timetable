#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

bool hashmapScheduleIdEquals(void *key_a, void *key_b) {
    return memcmp(key_a, key_b, sizeof (struct ScheduleId));
}

/* use djb hash unless we find it inadequate */
int hashmapScheduleIdHash(void *str) {
    uint32_t hash = 5381;
    char *p = str;

    for (int i = sizeof (struct ScheduleId); i > 0; i--)
        hash = ((hash << 5) + hash) + *p++;

    return (int) hash;
}
