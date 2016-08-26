
/* 
 * Timetable API daemon
 */

#include <stdio.h>
#include <stdlib.h>
#include <area51/json.h>
#include <area51/log.h>
#include <area51/hashmap.h>
#include <area51/rest.h>
#include <networkrail/timetable.h>
#include <networkrail/timetable/api.h>

#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <stdbool.h>

#include <area51/webserver.h>

int verbose = 0;
char *database = NULL;

struct TimeTable *timetable;
static WEBSERVER *webserver;

static int about() {
    logconsole("Usage: timetabled [-ip4] [-ip6] [-p{port}] database\n");
    return EXIT_FAILURE;
}

static int parseargs(WEBSERVER *webserver, int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        char *s = argv[i];
        if (s[0] == '-' && s[1] != 0) {
            if (strncmp("-ip4", s, 4) == 0)
                webserver_enableIPv4(webserver);
            else if (strncmp("-ip6", s, 4) == 0)
                webserver_enableIPv6(webserver);
            else switch (s[1]) {
                    case 'p':
                        if (s[2])
                            webserver_setPort(webserver, atoi(&s[2]));
                        break;

                    case 'v':
                        verbose++;
                        break;

                    default:
                        logconsole("Unsupported option %s", s);
                        return about();
                }
        } else if (s[0]) {
            if (database) {
                return about();
            }
            database = s;
        }
    }

    if (webserver_getPort(webserver) < 1 || webserver_getPort(webserver) > 65535 || !database)
        return about();

    return 0;
}

static char *getfile(char *database, char *suffix) {
    char *db = genurl(database, suffix);
    logconsole("Opening %s", db);
    return db;
}

static int opendb() {
    char *db = getfile(database, TT_SUFFIX_META);
    FILE *f = fopen(db, "r");
    free(db);
    if (!f)
        return EXIT_FAILURE;
    fread(&timetable->header, sizeof (struct TTHeader), 1, f);
    tt_log_header("Timetable");

    tt_idmap_read(f);
    fclose(f);

    db = getfile(database, TT_SUFFIX_TIPLOC);
    int ret = tt_tiploc_read(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    db = getfile(database, TT_SUFFIX_SCHEDULES);
    ret = tt_schedule_load(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    db = getfile(database, TT_SUFFIX_ENTRIES);
    ret = tt_schedule_load_entries(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    db = getfile(database, TT_SUFFIX_INDEX);
    ret = tt_schedule_index_load(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    db = getfile(database, TT_SUFFIX_UID_INDEX);
    ret = tt_schedule_lookup_load(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    db = getfile(database, TT_SUFFIX_ASSOCIATIONS);
    ret = tt_schedule_load_associations(db);
    free(db);
    if (ret)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    timetable = timetable_new();

    webserver = webserver_new();

    int rc = parseargs(webserver, argc, argv);
    if (rc)
        return rc;

    rc = opendb();
    if (rc)
        return rc;

    webserver_set_defaults(webserver);

    // 3alpha & crs are the same thing
    webserver_add_search_str(webserver, "/tiploc/3alpha", tt_find_tiploc_by_crs);
    webserver_add_search_str(webserver, "/tiploc/crs", tt_find_tiploc_by_crs);

    webserver_add_search_str(webserver, "/tiploc/tiploc", tt_find_tiploc);
    webserver_add_search_int(webserver, "/tiploc/stanox", tt_find_tiploc_by_stanox);

    webserver_add_search_str(webserver, "/schedule/uid", tt_get_schedules_by_uid);
    webserver_add_search_str(webserver, "/schedule/stanox", tt_get_schedules_by_stanox);

    webserver_add_static(webserver, "/station", tt_api_station_index);
    webserver_add_search_str(webserver, "/station", tt_api_station);

    webserver_add_handler(webserver, "/search", tt_api_station_search, NULL);

    webserver_add_static(webserver, "/status", tt_api_status);

    logconsole("Starting webserver on port %d", webserver_getPort(webserver));
    webserver_start(webserver);

    while (1) {
        sleep(60);
    }
}
