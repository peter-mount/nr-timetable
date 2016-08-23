
/* 
 * File:   tiploc.h
 * Author: peter
 *
 * Created on 02 August 2016, 08:00
 */

#ifndef NR_TT_TIPLOC_H
#define NR_TT_TIPLOC_H

#include <area51/hashmap.h>
#include <area51/list.h>
#include <area51/charbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TIPLOC_LENGTH 7
#define CRS_LENGTH 3

    struct TTTiploc {
        short id;
        char tiploc[TIPLOC_LENGTH + 1];
        char crs[CRS_LENGTH + 1];
        int stanox;
        int nlc;
        char nlc_check;
        char desc[27];
        char nlcdesc[17];
    };

    extern void tt_tiploc_index();
    extern int tt_tiploc_read(char *filename);
    extern int tt_tiploc_write(char *filename);

    extern void tt_append_tiploc_field(CharBuffer *b, char *n, short tiploc);
    extern void tt_append_tiploc(CharBuffer *b, struct TTTiploc *tpl);
    extern void tt_append_tiploc_node(CharBuffer *b, Node *n);

#ifdef __cplusplus
}
#endif

#endif /* TIPLOC_H */

