#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/hashmap.h>
#include <area51/log.h>
#include <area51/cif.h>

// Dummy handler used to implement an unsupported record type once we've recorded the fact

static int nop(struct CIFParser *p) {
    return EXIT_SUCCESS;
}

static int cif_parseNextRow(struct CIFParser *p) {

    char *s = p->line, *e = &p->file[p->fileLength];
    if (s >= e) {
        logconsole(__FILE__ ":%d s>e %lx %lx", __LINE__, s, e);
        return EXIT_FAILURE;
    }

    int q = 0;
    while (s < e && q < 120 && *s != '\n')
        p->buf[q++] = *s++;
    p->buf[q] = 0;

    if (q < 2) {
        logconsole(__FILE__ ":%d q<2 %d", __LINE__, q);
        return EXIT_FAILURE;
    }

    p->lineno++;

    if (s < e)
        p->line = *s == '\n' ? (s + 1) : s;
    else p->line = e;

    char cmd[3];
    cif_readString(p->buf, 0, cmd, 2);

    int (*handler)(struct CIFParser * p) = hashmapGet(p->handlers, cmd);

    if (handler) {
        int rc = handler(p);
        if (rc)
            logconsole("Handler %s failed line %d\n%s", cmd, p->lineno, p->buf);
        return rc;
    } else {
        logconsole("Unsupported record type \"%s\"", cmd);
        hashmapPut(p->handlers, strdup(cmd), nop);
    }

    return EXIT_SUCCESS;
}

int cif_parse(struct CIFParser *p, char *file, int len) {
    p->file = file;
    p->fileLength = len;
    p->line = file;
    p->lineno = 0;

    int rc = EXIT_SUCCESS;
    while (p->line < &p->file[p->fileLength] && !rc)
        rc = cif_parseNextRow(p);

    return rc;
}
