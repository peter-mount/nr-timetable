/*
 * Used to pre-scan a file list to generate the work to be done.
 * 
 * Specifically will scan the CIF headers, order the files and make certain
 * the full sequence is available. Any updated earlier than the last full
 * file will be removed from the list.
 * 
 */

/* 
 * File:   files.h
 * Author: peter
 *
 * Created on 10 August 2016, 11:28
 */

#ifndef NR_TTT_FILES_H
#define NR_TTT_FILES_H

#include <time.h>
#include <stdbool.h>
#include <area51/list.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct CifFile {
        // node.name will hold the file name
        Node node;
        time_t time;
        bool full;
    };

    /**
     * Scan a list containing CifFile entries. This will populate them with
     * the required meta data, and sort them. Any invalid ones will be removed
     * calling free() against the entry.
     * 
     * @param list
     * @return 1 on error
     */
    extern int cif_file_scan(List *list);

    /**
     * Add a file to the list
     * @param list
     * @param filename
     * @return 1 on error
     */
    extern int cif_file_add(List *list, char *filename);
    
#ifdef __cplusplus
}
#endif

#endif /* NR_TTT_FILES_H */

