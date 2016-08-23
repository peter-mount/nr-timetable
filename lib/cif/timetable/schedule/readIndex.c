#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/hashmap.h>
#include <area51/cif.h>
#include <area51/log.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/schedule.h>
#include <area51/stream.h>
#include <area51/string.h>
#include <area51/comparator.h>

struct ctx {
    int remain;
    int *p;
};

static void supplier(StreamData *d) {
    struct ctx *ctx = stream_getTaskContext(d);
    if (ctx && ctx->remain > 0) {
        ctx->remain--;
        stream_setVal(d, ctx->p++, NULL);
        stream_next(d);
    } else
        stream_terminate(d);
}

static Stream *flatMap(StreamData *d, void *c) {
    struct ctx *ctx = malloc(sizeof (struct ctx));
    if (!ctx)
        return NULL;

    int *ary = stream_getVal(d);
    ctx->remain = ary[0];
    ctx->p = &ary[1];

    Stream *s = stream_new();
    if (!s) {
        free(ctx);
        return NULL;
    }

    if (stream_invoke_r(s, supplier, ctx, free, "tt_stanox_schedule_flatMap")) {
        free(ctx);
        stream_free(s);
        return NULL;
    }

    return s;
}

/*
 * Flatmap the stanox-sched index into a stream of scheduleId's
 */
int tt_stanox_schedule_flatMap(Stream *s) {
    return stream_flatMap(s, flatMap, NULL);
}

static bool read(Hashmap *m, FILE *f) {
    // header[0] stanox
    // header[1] size
    int header[2], *r;
    fread(&header, sizeof (int), 2, f);
    r = malloc(sizeof (int)*(2 + header[1]));
    if (r) {
        r[0] = header[0];
        r[1] = header[1];
        fread(&r[2], sizeof (int), header[1], f);
        hashmapPut(m, &r[0], &r[1]);
        return true;
    } else
        return false;
}

int tt_schedule_index_load(char *filename) {

    FILE *f = fopen(filename, "r");
    if (!f)
        return EXIT_FAILURE;

    hashmapRead(timetable->schedStanox, read, f);
    fclose(f);

    logconsole(TT_LOG_FORMAT_D, "Stanox-Schedule", hashmapSize(timetable->schedStanox));

    return EXIT_SUCCESS;
}
