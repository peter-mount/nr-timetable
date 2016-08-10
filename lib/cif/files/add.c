#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/list.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <networkrail/timetable/files.h>

#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define MIN_FILE_SIZE 81

int cif_file_add(struct List *list, char *filename) {

    int fsock = open(filename, O_RDONLY);
    if (fsock == -1) {
        logconsole("Cannot open %s", filename);
        return EXIT_FAILURE;
    }

    struct stat sb;

    if (fsock == -1 || fstat(fsock, &sb) == -1) {
        close(fsock);
        return EXIT_FAILURE;
    }

    if (sb.st_size < MIN_FILE_SIZE) {
        close(fsock);
        // Return 0 as file is just not a valid CIF so don't include
        return 0;
    }

    char *fmap = (char *) mmap(NULL, MIN_FILE_SIZE, PROT_READ, MAP_PRIVATE, fsock, 0);
    if (fmap == MAP_FAILED) {
        close(fsock);
        // Fail here as we couldn't read it
        logconsole("Failed to read %s", filename);
        return EXIT_FAILURE;
    }

    // Valid CIF?
    bool valid = fmap[0] == 'H' && fmap[1] == 'D';

    if (valid) {
        valid = fmap[46] == 'F' || fmap[46] == 'U';
    }

    time_t time;
    if (valid) {
        struct tm tm;
        //time = cif_readDate_ddmmyyhhmm(fmap, 22, &tm);
        time = cif_readDate_ddmmyy(fmap, 48, &tm);
        valid = time > 100000;
    }
    
    if (valid) {
        // Add the file
        struct CifFile *file = (struct CifFile*) malloc(sizeof (struct CifFile));
        if (!file)
            valid = false;
        else {
            memset(file, 0, sizeof (struct CifFile));
            file->node.name = filename;
            file->time = time;
            file->full = fmap[46] == 'F';
            list_addTail(list, &file->node);
        }
    }

    munmap((void *) fmap, MIN_FILE_SIZE);
    close(fsock);
    return valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
