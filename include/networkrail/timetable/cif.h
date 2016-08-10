
/* 
 * File:   cif.h
 * Author: peter
 *
 * Created on 01 August 2016, 11:23
 */

#ifndef NRTT_CIF_H
#define NRTT_CIF_H

#include <area51/cif.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern int tt_parse_header(struct CIFParser *p);
    extern int tt_parse_trailer(struct CIFParser *p);
    
    extern int tt_parse_ttinsert(struct CIFParser *parser);
    extern int tt_parse_ttdelete(struct CIFParser *parser);

    extern int tt_parse_assoc(struct CIFParser *parser);

    extern int tt_parse_basic_schedule(struct CIFParser *parser);
    extern int tt_parse_loc_origin(struct CIFParser *parser);
    extern int tt_parse_loc_intermediate(struct CIFParser *parser);
    extern int tt_parse_loc_terminating(struct CIFParser *parser);
    
#ifdef __cplusplus
}
#endif

#endif /* NRTT_CIF_H */

