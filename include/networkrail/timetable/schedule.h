
/* 
 * File:   schedule.h
 * Author: peter
 *
 * Created on 02 August 2016, 08:00
 */

#ifndef NR_TT_SCHEDULE_H
#define NR_TT_SCHEDULE_H

#include <time.h>
#include <stdbool.h>

#include <networkrail/timetable/tiploc.h>
#include <networkrail/timetable/cif.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TT_UID_LENGTH 6

    // The Schedule Primary Key

    struct ScheduleId {
        char uid[TT_UID_LENGTH + 1];
        time_t start;
        char stpInd;
    };

    extern bool hashmapScheduleIdEquals(void *key_a, void *key_b);
    extern int hashmapScheduleIdHash(void *str);
    extern int tt_parse_scheduleId(struct CIFParser *parser, struct ScheduleId *id);
    extern int tt_schedule_write(Hashmap *m, char *filename);
    extern int tt_schedule_load(Hashmap *m, char *filename);
    
    // Association

    struct Assoc {
        struct ScheduleId id;
        char tiploc[TIPLOC_LENGTH + 1];
        char baseLocSuffix;
        char assocLocSuffix;
        char assocType;
        char days;
        char cat;
        char dateInd;
    };

    // CIF User Spec v29 FINAL Page 20,21,23
    // TODO P22 change en-route

    struct ScheduleEntry {
        //struct Node node;
        // "O", "I", "T" for Origin etc, i.e. LO, LI, LT
        char type;
        short tiploc;
        // Tiploc sequence (for circular routes)
        char tiplocseq;
        //char tiploc[9];
        // Public timetable in seconds of day
        int pta;
        int ptd;
        // Working timetable in seconds of day
        int wta;
        int wtd;
        int wtp;
        //
        char platform[4];
        char line[4];
        char path[4];
        // 6x2
        char activity[6][3];
        char engAllow[3];
        char pathAllow[3];
        char perfAllow[3];
        // reserved for Thameslink: LO and LT 3 chars, LI 5 chars
        char resThameslink[6];
    };

    struct Schedule {
        //struct Node node;
        struct ScheduleId id;
        //struct List entries;
        time_t end;
        char daysRun;
        char bankHolRun;
        char status[2];
        char category[3];
        char headcode[5];
        char serviceCode[2];
        char portionId[2];
        /*
        char powerType[4];
        char timingLoad[5];
        int speed;
        char characteristics[7];
         */
        // BX code here?
        int numEntries;
        struct ScheduleEntry *entries;
    };

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULE_H */

