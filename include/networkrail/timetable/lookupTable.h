/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lookupTable.h
 * Author: peter
 *
 * Created on 15 August 2016, 12:54
 */

#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include <area51/charbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern int ttref_parse_bankHoliday(char v);
    extern char *ttref_print_bankHoliday(int id);

    extern int ttref_parse_catering(char *v);
    extern void ttref_print_catering(struct charbuffer *b, int dr);
    
    extern int ttref_parse_daysRun(char *s);
    extern void ttref_print_daysRun(struct charbuffer *b, int dr);

    extern int ttref_parse_opchar(char *v);
    extern void ttref_print_opchar(struct charbuffer *b, int dr);
    
    extern int ttref_parse_portionId(char v);
    extern char ttref_print_portionId(int id);

    extern int ttref_parse_powerType(char a, char b, char c);
    extern char *ttref_print_powerType(int id);

    extern int ttref_parse_reservations(char v);
    extern char *ttref_print_reservations(int id);

    extern int ttref_parse_sleepers(char v);
    extern char *ttref_print_sleepers(int id);

    extern int ttref_parse_status(char v);
    extern void ttref_print_status(struct charbuffer *b,int id);

    extern int ttref_parse_trainCategory(char a, char b);
    extern char *ttref_print_trainCategory(int id);

#ifdef __cplusplus
}
#endif

#endif /* LOOKUPTABLE_H */

