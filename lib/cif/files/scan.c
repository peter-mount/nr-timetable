#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/list.h>
#include <networkrail/timetable/files.h>

/**
 * Compare two CifFile nodes. Sort them by time and when they have the same
 * time then updates come after full imports
 * @param a
 * @param b
 * @return 
 */
static int comparator(Node *a, Node *b) {
    struct CifFile *af = (struct CifFile *) a;
    struct CifFile *bf = (struct CifFile *) b;

    if (af->time > bf->time) {
        return 1;
    }

    if (af->time < bf->time) {
        return -1;
    }

    return af->full ? -11 : af->full == bf->full ? 0 : 1;
}

static void resolveFullImport(List *list) {
    bool full = false;
    
    // Find the last (if any) full import then remove all before it
    struct CifFile *fullFile;
    Node *n = list_getTail(list);
    while (!full && list_isNode(n)) {
        fullFile = (struct CifFile *) n;
        full = fullFile->full;
        n = list_getPred(n);
    }
    
    
    if (full) {
        // Remove all nodes before it as no point
        while (list_isNode(n)) {
            Node *p = list_getPred(n);
            list_remove(n);
            free(n);
            n = p;
        }

        // Check next node. If same time but an update then remove it as well
        n = list_getNext(&fullFile->node);
        if (list_isNode(n) && fullFile->time == ((struct CifFile *) n)->time) {
            list_remove(n);
            free(n);
        }
    }
    
}
int cif_file_scan(List *list) {

    // sort the list by order of their CIF import times
    list_sort(list, comparator);

    // Resolve if we are a full import and remove updates prior to that import
    resolveFullImport(list);

    return 0;
}
