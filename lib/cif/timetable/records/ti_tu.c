#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/hashmap.h>
#include <area51/cif.h>
#include <area51/log.h>

/*
 * Handler to handle both TI (Insert) and TU (update) records.
 * 
 * These records are identical except for update has possible additional entry for renaming a tiploc
 */
int tt_parse_ttinsert(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    int insert = 0;

    // Lookup for existing entry (update) or insert)
    char tiploc[8];
    cif_readString(parser->buf, 2, tiploc, 7);
    struct TTTiploc *tpl = (struct TTTiploc *) hashmapGet(p->loc, tiploc);
    if (!tpl) {
        tpl = (struct TTTiploc *) malloc(sizeof (struct TTTiploc));

        if (tpl) {
            memset(tpl, 0, sizeof (struct TTTiploc));
            insert = 1;
        }
    }

    // Still no tpl then fail - out of memory most likely
    if (!tpl)
        return EXIT_FAILURE;

    int ofs = 2;

    ofs = cif_readString(parser->buf, ofs, tpl->tiploc, 7);

    // ignore caps?
    ofs += 2;

    ofs = cif_readInt_r(parser->buf, ofs, &tpl->nlc, 6);
    tpl->nlc_check = parser->buf[ofs++];

    ofs = cif_readString(parser->buf, ofs, tpl->desc, 26);

    ofs = cif_readInt_r(parser->buf, ofs, &tpl->stanox, 5);

    // PO MCP code ignored
    ofs += 4;

    ofs = cif_readString(parser->buf, ofs, tpl->crs, 3);
    
    ofs = cif_readString(parser->buf, ofs, tpl->nlcdesc, 16);

    if (insert || parser->buf[ofs] == ' ') {
        struct TTTiploc *old = (struct TTTiploc *) hashmapPut(p->loc, tpl->tiploc, tpl);
        if (old) {
            tpl->id = old->id;
            free(old);
        } else {
            tpl->id = p->locSeq++;
        }
    } else {
        char newt[8];
        cif_readString(parser->buf, ofs, newt, 7);
        logconsole("Tiploc change from '%s' to '%s'", tpl->tiploc, newt);

        // Remove old entry from the map
        struct TTTiploc *old = (struct TTTiploc *) hashmapRemove(p->loc, tpl->tiploc);
        if (old) {
            tpl->id = old->id;
            free(old);
        } else {
            tpl->id = p->locSeq++;
        }

        // Replace new tiploc & re-insert
        cif_readString(parser->buf, ofs, tpl->tiploc, 7);
        old = (struct TTTiploc *) hashmapPut(p->loc, tpl->tiploc, tpl);
        if (old)
            free(old);
    }

    return EXIT_SUCCESS;
}
