
/* 
 * File:   cif.h
 * Author: peter
 *
 * Created on 01 August 2016, 08:00
 */

#ifndef AREA51_CIF_H
#define AREA51_CIF_H

#include <time.h>
#include <area51/hashmap.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct CIFParser {
        // Map holding record parsers
        Hashmap *handlers;
        // Input buffer
        char buf[128];
        // The data being read
        char *file;
        int fileLength;
        // Position in the file
        char *line;
        int lineno;
    };

    extern int cif_initParser(struct CIFParser *p);
    extern void cif_freeParser(struct CIFParser *p);
    extern int cif_parse(struct CIFParser *p, char *file, int len);
    extern int cif_parseFile(struct CIFParser *parser, int fsock);

    extern int cif_readString(char *src, int offset, char *dst, int len);
    extern int cif_readInt(char *src, int offset, int len);
    extern int cif_readInt_r(char *src, int offset, int *dst, int len);

    extern time_t cif_readDate(char *src, int offset, struct tm *tm, const char *fmt, int len);
    extern time_t cif_readDate_ddmmyy(char *src, int offset, struct tm *tm);
    extern time_t cif_readDate_yymmdd(char *src, int offset, struct tm *tm);
    extern time_t cif_readDate_ddmmyyhhmm(char *src, int offset, struct tm *tm);

    extern time_t cif_readtime_hhmm(char *src, int offset, struct tm *tm);
    
    extern int cif_readSeconds_hhmm(char *src, int offset);
    extern int cif_readSeconds_hhmm_r(char *src, int offset, int *dst);
    
    extern int cif_readSeconds_hhmmss(char *src, int offset);
    extern int cif_readSeconds_hhmmss_r(char *src, int offset, int *dst);

    extern int cif_readSeconds_hhmmh(char *src, int offset);
    extern int cif_readSeconds_hhmmh_r(char *src, int offset, int *dst);


#ifdef __cplusplus
}
#endif

#endif /* AREA51_CIF_H */

