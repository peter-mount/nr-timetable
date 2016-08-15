#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
#include <networkrail/timetable/lookupTable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/cif.h>
#include <area51/log.h>

static int delete(struct TimeTable *p, struct ScheduleId *sid) {
    void *old = hashmapRemove(p->schedules, sid);
    if (old) {
        purgeExpiredSchedule((struct Schedule *) old);
    }
    return EXIT_SUCCESS;
}

static int insert(struct TimeTable *p, struct ScheduleId *sid) {
    struct tm tm;

    struct Schedule *s = (struct Schedule *) malloc(sizeof (struct Schedule));
    if (!s)
        return EXIT_FAILURE;

    // Copy the scheduleId and link the node name to the copy
    memcpy(&s->id, sid, sizeof (struct ScheduleId));
    s->numEntries = 0;

    // Copy of buffer pointer
    char *b = p->parser.buf;
    int o = 3 + TT_UID_LENGTH + 6;
    o = cif_readDate_yymmdd_r(b, o, &tm, &s->end);

    s->daysRun = ttref_parse_daysRun(&b[o]);
    o += 7;

    if (b[o] != ' ') {
        s->bankHolRun = ttref_parse_bankHoliday(b[o]);
        if (!s->bankHolRun)
            logconsole("Unsupported bankHolRun '%c'", b[o]);
    }
    o++;

    if (b[o] != ' ') {
        s->status = ttref_parse_status(b[o]);
        if (!s->status)
            logconsole("Unsupported status '%c'", b[o]);
    }
    o++;

    if (b[o] != ' ' && b[o + 1] != ' ') {
        s->category = ttref_parse_trainCategory(b[o], b[o + 1]);
        if (!s->category)
            logconsole("Unsupported train category '%c%c'", b[o], b[o + 1]);
    }
    o += 2;

    o = cif_readString(b, o, s->trainId, 4);
    o = cif_readString(b, o, s->headcode, 4);

    // Course indicator, not used
    o++;

    o = cif_readInt_r(b, o, &s->serviceCode, 8);

    if (b[o] != ' ') {
        s->portionId = ttref_parse_portionId(b[o]);
        if (!s->portionId)
            logconsole("Unsupported portionId '%c'", b[o]);
    }
    o++;

    if (b[o] != ' ' && b[o + 1] != ' ' && b[o + 2] != ' ') {
            s->powerType = ttref_parse_powerType(b[o], b[o + 1], b[o + 2]);
            if (!s->powerType)
                logconsole("Unsupported powerType '%c%c%c'", b[o], b[o + 1], b[o + 2]);
        }
    o += 3;

    o = cif_readString(b, o, s->timingLoad, 4);
    o = cif_readInt_r(b, o, &s->speed, 3);

    o += 6; // operating characteristics

    o++; // seating class
    o++; // sleepers
    o++; // reservations
    o++; // not used - connection indicator
    o += 4; // catering code
    o += 4; // service branding

    // Add to collection and make it current
    void *old = hashmapPut(p->schedules, &s->id, s);
    if (old) {
        purgeExpiredSchedule((struct Schedule *) old);
    }

    p->curschedule = s;
    return EXIT_SUCCESS;
}

/*
 * Parse BasicSchedule (BS) record. First record in a schedule
 */
int tt_parse_basic_schedule(struct CIFParser *parser) {
    struct TimeTable *p = (struct TimeTable *) parser;

    // Complete any previous schedule
    tt_parse_complete_schedule(p);

    char ttype = parser->buf[2];
    struct ScheduleId id;
    if (tt_parse_scheduleId(parser, &id))
        return EXIT_FAILURE;

    int rc = EXIT_FAILURE;
    switch (ttype) {

        case 'N':
            rc = insert(p, &id);
            break;

        case 'D':
            rc = delete(p, &id);
            break;

        case 'R':
            // Replace, so delete and insert the new schedule
            rc = delete(p, &id);
            if (!rc)
                rc = insert(p, &id);
            break;

        default:
            logconsole("Unsupported BS type %c for %s %c line %d", ttype, id.uid, id.stpInd, parser->lineno);
            rc = EXIT_FAILURE;
            break;
    }

    return rc;
}
