#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/cif.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>

struct TimeTable *timetable_new() {
    struct TimeTable *tt = (struct TimeTable *) malloc(sizeof (struct TimeTable));
    memset(tt, 0, sizeof (struct TimeTable));

    tt->loc = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->assoc = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->schedules = hashmapCreate(100, hashmapScheduleIdHash, hashmapScheduleIdEquals);

    // api only
    
    tt->idTiploc = hashmapCreate(100, hashmapShortHash, hashmapShortEquals);
    tt->crsTiploc = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->stanoxTiploc = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    
    tt->uid = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    
    // see idmap.c
    tt->idmap = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->txtmap = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    
    return tt;
}
