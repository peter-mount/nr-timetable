#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <area51/cif.h>
#include <area51/log.h>

#include "networkrail/timetable/lookupTable.h"

/*
 * Initialise struct Assoc and parse common header values into it.
 *
 * @param parser CIFParser
 * @param a AssocID to populate
 * @return offset in buffer to first field after the header
 */
static int parseCommon(struct CIFParser *parser, struct AssocID *a) {
    struct tm tm;
    memset(a, 0, sizeof (struct AssocID));
    
    // First three chars are AAx where X is D, N or R
    int o = 3;
    
    o = cif_readString(parser->buf, o, a->mainUid, 6);
    o = cif_readString(parser->buf, o, a->assUid, 6);
    o = cif_readDate_yymmdd_r(parser->buf, o, &tm, &a->start);
    o = cif_readDate_yymmdd_r(parser->buf, o, &tm, &a->end);

    // stpInd is the last char of the record
    a->stpInd = parser->buf[79];

    return o;
}

static void removeAssociation(char *uid, struct AssocID *a) {
    List *l = hashmapGet(timetable->assoc, uid);
    if (l) {
        Node *n = list_getHead(l);
        while (list_isNode(n)) {
            Node *next = list_getNext(n);

            struct AssocID *b = n->value;
            if (strcmp(a->mainUid, b->mainUid) == 0 &&
                    strcmp(a->assUid, b->assUid) == 0 &&
                    a->start == b->start &&
                    a->end == b->end &&
                    a->stpInd == b->stpInd
                    ) {
                // This will remove and free the Assoc structure
                list_remove(n);
                node_free(n);
            }

            n = next;
        }
    }
}

static int delete(struct CIFParser *parser) {
    struct AssocID a;
    parseCommon(parser, &a);
    removeAssociation(a.mainUid, &a);
    removeAssociation(a.assUid, &a);
    return EXIT_SUCCESS;
}

static int insert(struct CIFParser *parser) {
    struct TimeTable *t = (struct TimeTable *) parser;

    struct Assoc *a = malloc(sizeof (struct Assoc));
    if (!a)
        return EXIT_FAILURE;

    memset(a, 0, sizeof (struct Assoc));
    int o = parseCommon(parser, &a->id);

    // association days
    a->daysRun = ttref_parse_daysRun(&parser->buf[o]);
    o += 7;

    // association category
    char cat[3];
    o = cif_readString(parser->buf, o, cat, 2);
    switch (cat[0]) {
        // '  ' Present for Cancelled associations
        // 0 here as cif_readString trims the spaces
        case 0:
            a->category = 0;
            break;
            
            // JJ
        case 'J':
            a->category = 1;
            break;

            // VV
        case 'V':
            a->category = 2;
            break;

            // NP
        case 'N':
            a->category = 3;
            break;

            // Anything else - should never occur
        default:
            // Free the new entry, warn and exit but continue parsing
            logconsole("Unsupported association category %d \"%s\" on %s->%s", cat[0], cat, a->id.mainUid, a->id.assUid);
            free(a);
            return EXIT_SUCCESS;
    }

    // date ind
    char dind = parser->buf[o++];
    switch (dind) {
        // ' ' on cancelled associations
        case ' ':
            a->dateInd = 0;
            break;

            // S Standard (same day)
        case 'S':
            a->dateInd = 1;
            break;

            // N over-next-midnight
        case 'N':
            a->dateInd = 2;
            break;

            // P over-previous-midnight
        case 'P':
            a->dateInd = 3;
            break;

            // Free the new entry, warn and exit but continue parsing
        default:
            logconsole("Unsupported association dateInd \"%c\" on %s->%s", dind, a->id.mainUid, a->id.assUid);
            free(a);
            return EXIT_SUCCESS;
    }

    // association location
    char tiploc[10];
    o = cif_readString(parser->buf, o, tiploc, 7);
    struct TTTiploc *tpl = tt_getTiploc(t, tiploc);
    a->tiploc = tpl->id;

    // baseSeq - CIF spec says ' ' or '2' but we support 0-9 here
    dind = parser->buf[o++];
    if (dind != ' ') a->baseSeq = dind - '0';

    // assocSeq - CIF spec says ' ' or '2' but we support 0-9 here
    dind = parser->buf[o++];
    if (dind != ' ') a->assocSeq = dind - '0';

    // Diagram type - ignored as CIF spec says unused
    o++;

    // Association type: P or S so for us anything other than P is true
    dind = parser->buf[o++];
    a->type = dind != 'P';

    // Add it to both uid's association lists
    hashmapAddList(timetable->assoc, a->id.mainUid, a);
    hashmapAddList(timetable->assoc, a->id.assUid, a);
    return EXIT_SUCCESS;
}

int tt_parse_assoc(struct CIFParser *parser) {
    switch (parser->buf[2]) {
            // Delete association
        case 'D':
            return delete(parser);

            // New & Revise are same operation, remove existing (if any) then insert
        case 'N':
        case 'R':
            delete(parser);
            return insert(parser);

        default:
            logconsole("Unknown association transaction type %c", parser->buf[2]);
            return EXIT_FAILURE;
    }
}
