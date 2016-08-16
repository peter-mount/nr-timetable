
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

    // Bits required to store time of day
#define TIME_KEY_SIZE 17

    // CIF User Spec v29 FINAL Page 20,21,23
    // TODO P22 change en-route

    struct ScheduleEntry {
        //struct Node node;
        // "O", "I", "T" for Origin etc, i.e. LO, LI, LT
        unsigned int type : 2;
        // Tiploc (currently 10k) room for 500 entries before increasing this
        unsigned int tiploc : 14;
        // Tiploc sequence (for circular routes), 0-9
        unsigned int tiplocseq : 4;

        // Public timetable in seconds of day
        unsigned int pta : TIME_KEY_SIZE;
        unsigned int ptd : TIME_KEY_SIZE;
        // Working timetable in seconds of day
        unsigned int wta : TIME_KEY_SIZE;
        unsigned int wtd : TIME_KEY_SIZE;
        unsigned int wtp : TIME_KEY_SIZE;
        //
        unsigned long activity : 36;
        unsigned int platform : IDKEY_SIZE;
        unsigned long line : IDKEY_SIZE;

        unsigned long path : IDKEY_SIZE;
        unsigned long engAllow : IDKEY_SIZE;
        unsigned long pathAllow : IDKEY_SIZE;
        unsigned long perfAllow : IDKEY_SIZE;
    };

    struct Schedule {
        //struct Node node;
        struct ScheduleId id;
        time_t end;

        // 0000-9999
        unsigned int headcode : 14;

        unsigned long trainId : IDKEY_SIZE;

        unsigned int daysRun : 7;
        // first & standard class if set, standard class only if false
        unsigned int firstClass : 1;

        unsigned int category : 6;
        unsigned int status : 4;
        unsigned int bankHolRun : 2;
        unsigned int portionId : 4;

        unsigned int powerType : 4;
        unsigned int sleepers : 2;

        unsigned int reservations : 3;
        unsigned int catering : 12;

        unsigned int operatingCharacteristics : 24;
        unsigned long timingLoad : IDKEY_SIZE;

        unsigned int serviceCode : 27;
        unsigned int speed : 10;

        // Max numEntries seen is 150
        unsigned int numEntries : 8;
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

    extern void tt_schedules_result(struct charbuffer *b, struct List *l, bool (*filter)(struct Node *n,void *c),void *c);
    extern void tt_get_schedules_by_uid(struct charbuffer *b, const char *tiploc);

#ifdef __cplusplus
}
#endif

#endif /* SCHEDULE_H */

