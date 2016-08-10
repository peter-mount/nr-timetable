#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/cif.h>

static const char *FMT_HHMM = "%H%M";
static const char *FMT_HHMMSS = "%H%M%S";

/**
 * Convert CIF time HHMM into seconds of the day
 * 
 * Note: This cannot handle leap seconds but then leap seconds don't appear in CIF so not an issue in real life
 * 
 * @param src
 * @param offset
 * @return 
 */
int cif_readSeconds_hhmm(char *src, int offset) {
    if (src[offset] == ' ' || src[offset + 1] == ' ' || src[offset + 2] == ' ' || src[offset + 3] == ' ')
        return 0;

    struct tm tm;
    memset(&tm, 0, sizeof (struct tm));
    cif_readDate(src, offset, &tm, FMT_HHMM, 4);
    return (tm.tm_hour * 3600)+(tm.tm_min * 60);
}

int cif_readSeconds_hhmm_r(char *src, int offset, int *dst) {
    *dst = cif_readSeconds_hhmm(src, offset);
    return offset + 4;
}

/**
 * Convert CIF time HHMMSS into seconds of the day
 * 
 * Note: This cannot handle leap seconds but then leap seconds don't appear in CIF so not an issue in real life
 * 
 * @param src
 * @param offset
 * @return 
 */
int cif_readSeconds_hhmmss(char *src, int offset) {
    if (src[offset] == ' ' || src[offset + 1] == ' ' || src[offset + 2] == ' ' || src[offset + 3] == ' ' || src[offset + 4] == ' ' || src[offset + 5] == ' ')
        return 0;

    struct tm tm;
    memset(&tm, 0, sizeof (struct tm));
    cif_readDate(src, offset, &tm, FMT_HHMM, 4);
    return (tm.tm_hour * 3600)+(tm.tm_min * 60) + tm.tm_sec;
}

int cif_readSeconds_hhmmss_r(char *src, int offset, int *dst) {
    *dst = cif_readSeconds_hhmmss(src, offset);
    return offset + 6;
}

/**
 * Convert CIF time HHMMH into seconds of the day.
 * 
 * In this instance we look for HHMM and if the 5th digit is 'H' then thats treated as a 30s otherwise 0s.
 * 
 * @param src
 * @param offset
 * @return 
 */
int cif_readSeconds_hhmmh(char *src, int offset) {
    return cif_readSeconds_hhmm(src, offset)+(src[offset + 4] == 'H' ? 30 : 0);
}

int cif_readSeconds_hhmmh_r(char *src, int offset, int *dst) {
    *dst = cif_readSeconds_hhmmh(src, offset);
    return offset + 5;
}
