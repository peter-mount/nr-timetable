#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/cif.h>

static const char *FMT_DMYHM = "%d%m%y%H%M";
static const int FMT_DMYHM_LEN = 10;

static const char *FMT_DMY = "%d%m%y";
static const int FMT_DMY_LEN = 6;

static const char *FMT_YMD = "%y%m%d";
static const int FMT_YMD_LEN = 6;

static const char *FMT_HM = "%H%M";
static const int FMT_HM_LEN = 4;

time_t cif_readDate(char *src, int offset, struct tm *tm, const char *fmt, int len) {
    char buf[len + 1];
    cif_readString(src, offset, buf, len);

    memset(tm, 0, sizeof (struct tm));
    strptime(buf, fmt, tm);

    return mktime(tm);
}

int cif_readDate_r(char *src, int offset, struct tm *tm, time_t *t, const char *fmt, int len) {
    *t = cif_readDate(src, offset, tm, fmt, len);
    return offset + len;
}

time_t cif_readDate_ddmmyyhhmm(char *src, int offset, struct tm *tm) {
    return cif_readDate(src, offset, tm, FMT_DMYHM, FMT_DMYHM_LEN);
}

int cif_readDate_ddmmyyhhmm_r(char *src, int offset, struct tm *tm, time_t *t) {
    return cif_readDate_r(src, offset, tm, t, FMT_DMYHM, FMT_DMYHM_LEN);
}

time_t cif_readDate_ddmmyy(char *src, int offset, struct tm *tm) {
    return cif_readDate(src, offset, tm, FMT_DMY, FMT_DMY_LEN);
}

int cif_readDate_ddmmyy_r(char *src, int offset, struct tm *tm, time_t *t) {
    return cif_readDate_r(src, offset, tm, t, FMT_DMY, FMT_DMY_LEN);
}

time_t cif_readDate_yymmdd(char *src, int offset, struct tm *tm) {
    return cif_readDate(src, offset, tm, FMT_YMD, FMT_YMD_LEN);
}

int cif_readDate_yymmdd_r(char *src, int offset, struct tm *tm, time_t *t) {
    return cif_readDate_r(src, offset, tm, t, FMT_YMD, FMT_YMD_LEN);
}

time_t cif_readTime_hhmm(char *src, int offset, struct tm *tm) {
    return cif_readDate(src, offset, tm, FMT_HM, FMT_HM_LEN);
}

int cif_readTime_hhmm_r(char *src, int offset, struct tm *tm, time_t *t) {
    return cif_readDate_r(src, offset, tm, t, FMT_HM, FMT_HM_LEN);
}
