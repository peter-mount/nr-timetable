
/* 
 * File:   timetable.h
 * Author: peter
 *
 * Created on 01 August 2016, 08:04
 */

#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <area51/list.h>
#include <area51/hashmap.h>
#include <area51/cif.h>

#include <networkrail/timetable/tiploc.h>
#include <networkrail/timetable/schedule.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TT_HEADER_MAINFRAMEID 20
#define TT_HEADER_FILE_REF 7

    struct TTHeader {
        // Time of import
        time_t importTime;
        char mainframeId[TT_HEADER_MAINFRAMEID + 1];
        char fileRef[TT_HEADER_FILE_REF + 1];
        char lastFileRef[TT_HEADER_FILE_REF + 1];
        time_t userStart;
        time_t userEnd;
    };

    // Linked Hashmap

    struct TimeTable {
        // =======================
        // Only used by ttimport
        // =======================
        struct CIFParser parser;
        struct Schedule *curschedule;
        int entryCount, numEntries;
        struct ScheduleEntry *entries;
        struct List *curentries;

        // =======================
        // Common to all code
        // =======================

        // The current schedule header
        struct TTHeader header;
        
        // Tiplocs
        short locSeq;
        Hashmap *loc;
        // Association data
        Hashmap *assoc;
        // Schedule data
        Hashmap *schedules;

        // =======================
        // Used in meta data to hold common strings, platform's etc
        // =======================
        Hashmap *idmap;
        Hashmap *txtmap;
        
        // =======================
        // Only used by timetabled
        // =======================
        
        Hashmap *idTiploc;
        
        // CRS->tiploc index
        Hashmap *crsTiploc;
        
        // Stanox->tiploc index. Entries are Lists as it's 1->n relationship
        Hashmap *stanoxTiploc;

        // Schedule UID. Entries are lists of entries
        Hashmap *uid;
    };

    extern void purgeExpiredSchedule(struct Schedule *s);
    extern void purgeExpiredSchedules(struct TimeTable *t);

    extern struct TimeTable *timetable_new();

    extern void timetable_initParser(struct TimeTable *tt);

    extern int timetable_load(struct TimeTable *tt, char *filename);
    extern int timetable_save(struct TimeTable *tt, char *filename);

    extern void tt_parse_ensure_capacity(struct TimeTable *t);
    extern void tt_parse_complete_schedule(struct TimeTable *t);
    extern struct ScheduleEntry *tt_parse_nextScheduleEntry(struct TimeTable *t);

    extern Hashmap *mapTiploc_new();
    extern void mapTiploc_addTiploc(Hashmap *m, short tiploc);
    extern void mapTiploc_appendIndex(struct charbuffer *b, Hashmap *m);
    
    extern struct TTTiploc *tt_getTiploc(struct TimeTable *t, char *tiploc);

    extern void tt_index();

    extern const char *ttref_portionId(char v);
    
    extern int tt_idmap_add(char *k);
    extern int tt_idmap_add_r(char *k, int o, int l);
    extern char *tt_idmap_get(int k);
    extern void tt_idmap_read(FILE *f);
    extern void tt_idmap_write(FILE *f);
    
#ifdef __cplusplus
}
#endif

#endif /* TIMETABLE_H */

