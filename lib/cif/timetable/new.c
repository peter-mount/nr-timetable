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
    tt->schedules = hashmapCreate(100, hashmapScheduleIdHash, hashmapScheduleIdEquals);
    tt->scheduleEntry = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    tt->scheduleSID = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);

    // Associations list on UID (2 entries, one for mainUid and assocUid)
    tt->assoc = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    
    // api only
    
    tt->idTiploc = hashmapCreate(100, hashmapShortHash, hashmapShortEquals);
    tt->crsTiploc = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->stanoxTiploc = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    
    tt->uid = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    // Map of schedules per stanox
    tt->schedStanox = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    // used by schedStanox, id->ScheduleId
    tt->schedId = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    // used when creating schedId & schedStanox
    //tt->idSched = hashmapCreate(100, hashmapScheduleIdHash, hashmapScheduleIdEquals);
    tt->idSched = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    
    // see idmap.c
    tt->idmap = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    tt->txtmap = hashmapCreate(100, hashmapIntHash, hashmapIntEquals);
    
    return tt;
}
