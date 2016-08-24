#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/cif.h>
#include <area51/file.h>
#include <area51/hashmap.h>
#include <area51/log.h>
#include <area51/string.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>

static bool writeAssocEntry(void *k, void *v, void *c) {
    FILE *f = (FILE *) c;
    List *l = v;

    // The number of entries - ignore the entry if empty
    int s = list_size(l);
    if (s) {

        char uid[TT_UID_LENGTH + 1];
        memset(uid, 0, TT_UID_LENGTH + 1);
        strncpy(uid, (char *) k, TT_UID_LENGTH);
        fwrite(uid, 1, TT_UID_LENGTH + 1, f);

        fwrite(&s, sizeof (int), 1, f);

        Node *n = list_getHead(l);
        while (list_isNode(n)) {
            struct Assoc *a = n->value;

            // Set the unique id
            a->id.id = timetable->aid++;

            fwrite(a, sizeof (struct Assoc), 1, f);

            n = list_getNext(n);
        }

    }

    return true;
}

/*
 * Writes the association table
 */
int tt_schedule_write_associations(char *filename) {

    backupFile(filename);

    FILE *f = fopen(filename, "w");
    if (!f)
        return EXIT_FAILURE;

    hashmapWrite(timetable->assoc, writeAssocEntry, f);
    fclose(f);
    return EXIT_SUCCESS;
}
