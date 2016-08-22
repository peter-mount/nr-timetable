
/* 
 * File:   corpus-import.c
 * Author: peter
 *
 * Created on 17 July 2016, 21:20
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <area51/json.h>
#include <area51/log.h>
#include <area51/cif.h>
#include <area51/list.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/files.h>

#include <ctype.h>

#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int verbose = 0;

struct TimeTable *timetable;

static int importCIF(const char *filename) {
    int fsock = open(filename, O_RDONLY);
    if (fsock == -1) {
        logconsole("Cannot open %s", filename);
        return EXIT_FAILURE;
    }

    logconsole(TT_LOG_FORMAT_S, "Importing", filename);
    int rc = cif_parseFile(&timetable->parser, fsock);

    close(fsock);

    if (rc) {
        logconsole("Import failed");
        return EXIT_FAILURE;
    }

    //logconsole("Import complete");
    return EXIT_SUCCESS;
}

static int about(char *cmd) {
    logconsole("Syntax:");
    logconsole("  %s db cif[...]", cmd);
    return EXIT_FAILURE;
}

static int scanFiles(int argc, char **argv, int arg, List *files) {
    logconsole("Scanning CIF files");

    for (; arg < argc; arg++) {
        cif_file_add(files, argv[arg]);
    }

    if (cif_file_scan(files)) {
        logconsole("Failed to scan supplied files");
        return EXIT_FAILURE;
    }

    if (list_isEmpty(files)) {
        logconsole("No CIF files to import");
        return EXIT_FAILURE;
    }

    logconsole("Found %d files to import", list_size(files));

    struct CifFile *first = (struct CifFile *) list_getHead(files);
    if (!first->full) {
        logconsole("A full CIF export is required but first file is an update");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        return about(argv[0]);
    }

    timetable = timetable_new();

    int arg = 1;
    char *newdb = argv[arg++];

    List files;
    list_init(&files);
    if (scanFiles(argc, argv, arg, &files))
        return EXIT_FAILURE;

    struct CifFile *first = (struct CifFile *) list_getHead(&files);

    // Import each cif in sequence
    timetable_initParser(timetable);
    Node *n = list_getHead(&files);
    while (list_isNode(n)) {
        if (importCIF(n->name))
            return EXIT_FAILURE;
        purgeExpiredSchedules(timetable);
        n = list_getNext(n);
    }

    // Create indices
    logconsole("Generating indices");
    tt_tiploc_index();
    tt_schedule_index_stanox();

    // Persist to disk
    timetable_save(timetable, newdb);

    logconsole("New timetables complete");
    return EXIT_SUCCESS;
}

