#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <networkrail/timetable.h>
#include <networkrail/timetable/cif.h>
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

    // Copy of buffer pointer
    char *b = p->parser.buf;

    s->end = cif_readDate_yymmdd(b, 3 + TT_UID_LENGTH + 6, &tm);
    s->numEntries = 0;

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
