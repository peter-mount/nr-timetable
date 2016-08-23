#include <area51/webserver.h>
#include <area51/string.h>
#include <string.h>
#include <limits.h>

#ifndef NRTT_API_H
#define NRTT_API_H

#ifdef __cplusplus
extern "C" {
#endif

    // /schedule/ services
    extern void tt_get_schedules_by_stanox(CharBuffer *, const char *);
    extern void tt_get_schedules_by_uid(CharBuffer *, const char *);

    // /status service
    extern void tt_api_status(CharBuffer *b);
    
    // /tiploc/ services
    extern void tt_find_tiploc(CharBuffer *b, const char *tiploc);
    extern void tt_find_tiploc_by_crs(CharBuffer *b, const char *crs);
    extern void tt_find_tiploc_by_stanox(CharBuffer *b, int stanox);

#ifdef __cplusplus
}
#endif

#endif /* NRTT_CIF_H */

