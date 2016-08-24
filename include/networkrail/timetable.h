
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

/*
 * Bits required to store idmap key, see below on definition & if/when to
 * increase this size
 */
#define IDKEY_SIZE 14

#include <networkrail/timetable/tiploc.h>
#include <networkrail/timetable/schedule.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TT_HEADER_MAINFRAMEID 20
#define TT_HEADER_FILE_REF 7

    // format used in logging number of entries when loading, e.g.
    //logconsole(TT_LOG_FORMAT_D, "Schedules", hashmapSize(m));

#define TT_LOG_FORMAT_P "%16s "
#define TT_LOG_FORMAT_D TT_LOG_FORMAT_P "%10d"
#define TT_LOG_FORMAT_S TT_LOG_FORMAT_P "%s"

    // The various file suffixes
#define TT_SUFFIX_META          ".mta"
#define TT_SUFFIX_TIPLOC        ".tpl"
#define TT_SUFFIX_SCHEDULES     ".sch"
#define TT_SUFFIX_ENTRIES       ".ent"
#define TT_SUFFIX_INDEX         ".stx"
#define TT_SUFFIX_UID_INDEX     ".uid"
#define TT_SUFFIX_ASSOCIATIONS  ".ass"

    struct TTHeader {
        // Time of import
        time_t importTime;
        // Time of extract by Network Rail
        time_t extractTime;
        // Mainframe Id of format TPS.Uxxxxxx.PDyymmdd
        // Where xxxxxx is username (should always be DFROC1)
        //       yymmdd is date extract relates to
        char mainframeId[TT_HEADER_MAINFRAMEID + 1];
        // File ref is xxxxxxa where xxxxxx should always be DFROC1)
        // and a is A-Z i.e. 26 updates before cycling back
        char fileRef[TT_HEADER_FILE_REF + 1];
        // As fileRef, the previous file (usually the previous day)
        char lastFileRef[TT_HEADER_FILE_REF + 1];
        // Start date the schedules cover
        time_t userStart;
        // End date the schedules cover
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
        List *curentries;

        // =======================
        // Common to all code
        // =======================

        // Unique ID in database for the schedule
        unsigned int sid;
        
        // Unique ID in database for associations
        unsigned int aid;

        // The current schedule header
        struct TTHeader header;

        // Tiplocs
        short locSeq;
        Hashmap *loc;
        
        // Association data
        // Parsing this contain's Lists, when running an array of data
        Hashmap *assoc;
        
        // Schedule data
        Hashmap *schedules;
        // ScheduleEntries
        Hashmap *scheduleEntry;
        Hashmap *scheduleSID;

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
        
        // Schedules at each stanox
        Hashmap *schedStanox;
        // scheduleUid used by schedStanox to reduce index size
        Hashmap *schedId;
        // Used only when creating schedStanox/schedId
        Hashmap *idSched;
    };

    extern struct TimeTable *timetable;

    extern void purgeExpiredSchedule(struct Schedule *s);
    extern void purgeExpiredSchedules(struct TimeTable *t);

    extern struct TimeTable *timetable_new();

    extern void timetable_initParser(struct TimeTable *tt);

    extern int timetable_save(struct TimeTable *tt, char *filename);

    extern void tt_log_header(char *task);
    extern void tt_parse_ensure_capacity(struct TimeTable *t);
    extern void tt_parse_complete_schedule(struct TimeTable *t);
    extern struct ScheduleEntry *tt_parse_nextScheduleEntry(struct TimeTable *t);

    extern Hashmap *mapTiploc_new();
    extern void mapTiploc_addTiploc(Hashmap *m, short tiploc);
    extern void mapTiploc_appendIndex(CharBuffer *b, Hashmap *m);

    extern struct TTTiploc *tt_getTiploc(struct TimeTable *t, char *tiploc);

    extern void tt_index();
//    extern void tt_schedule_load_uid();
//    extern void tt_schedule_sort();
    
    /*
     * The idmap/txtmap constant string pool.
     * 
     * This pool contains common strings like trainId, platform numbers etc.
     * It's size is limited by IDKEY_SIZE, which defines the number of bits
     * in the various tables. If this grows beyond 2^IDKEY_SIZE then we must
     * increase this size, forcing the table to grow as well.
     * 
     * In the varous structures define fields that link to this table as:-
     * 
     * unsigned int fieldname : IDKEY_SIZE;
     * 
     * Note: Key's are limited to 15 characters when stored on disk.
     */

    /**
     * Lookup entry adding as necessary
     * @param k entry to look
     * @return id in the map
     */
    extern int tt_idmap_add(char *k);
    /**
     * Lookup entry adding as necessary
     * @param k entry to look
     * @param o offset in k of start of key
     * @param l length of key
     * @return id in the map
     */
    extern int tt_idmap_add_r(char *k, int o, int l);
    /**
     * Retrieve the constant value
     * @param k id of constant
     * @return constant or NULL
     */
    extern char *tt_idmap_get(int k);
    /**
     * Read the constant table from a File
     * @param f FILE
     */
    extern void tt_idmap_read(FILE *f);
    /**
     * Write the constant table to a File
     * @param f FILE
     */
    extern void tt_idmap_write(FILE *f);

#ifdef __cplusplus
}
#endif

#endif /* TIMETABLE_H */

