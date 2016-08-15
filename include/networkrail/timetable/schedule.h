
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
        unsigned int type : 2;
        unsigned int tiploc : 14;
        // Tiploc sequence (for circular routes)
        unsigned int tiplocseq : 4;

        // Public timetable in seconds of day
        unsigned int pta : 17;
        unsigned int ptd : 17;
        // Working timetable in seconds of day
        unsigned int wta : 17;
        unsigned int wtd : 17;
        unsigned int wtp : 17;
        //
        unsigned long activity : 36;
        unsigned int platform : 14;
        unsigned long line : 14;

        unsigned long path : 14;
        unsigned long engAllow : 14;
        unsigned long pathAllow : 14;
        unsigned long perfAllow : 14;
    };

    struct Schedule {
        //struct Node node;
        struct ScheduleId id;
        time_t end;
        // Origin tiploc
        short origin;
        // destination tiploc
        short dest;

        char headcode[5];
        char trainId[5];

        int serviceCode;

        unsigned int daysRun : 7;
        // first & standard class if set, standard class only if false
        unsigned int firstClass : 1;

        unsigned int category : 6;
        unsigned int status : 4;
        unsigned int bankHolRun : 2;
        unsigned int portionId : 4;

        unsigned int powerType : 4;
        unsigned int sleepers : 2;
        unsigned int : 2;

        unsigned int reservations : 3;
        unsigned int catering : 12;
        unsigned int : 1;

        unsigned int operatingCharacteristics : 24;
        unsigned int : 8;

        char timingLoad[5];
        int speed;

        int numEntries;
        struct ScheduleEntry *entries;
    };

    extern bool hashmapScheduleIdEquals(void *key_a, void *key_b);
    extern int hashmapScheduleIdHash(void *str);
    extern int tt_parse_scheduleId(struct CIFParser *parser, struct ScheduleId *id);
    extern int tt_schedule_write(Hashmap *m, char *filename);
    extern int tt_schedule_load(Hashmap *m, char *filename);

    extern void tt_append_schedule(struct charbuffer *b, struct Schedule *e);
    extern void tt_append_schedule_node(struct charbuffer *b, struct Node *n);

    extern void tt_append_scheduleEntry(struct charbuffer *b, struct ScheduleEntry *e);

    extern void append_hhmm(struct charbuffer *b, char *n, int tm);
    extern void append_hhmmss(struct charbuffer *b, char *n, int tm);

    extern void mapTiploc_mapSchedule(Hashmap *m, struct Schedule *s);
    extern void mapTiploc_mapScheduleEntry(Hashmap *m, struct ScheduleEntry *e);

    extern void tt_get_schedules_by_uid(struct charbuffer *b, const char *tiploc);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULE_H */

