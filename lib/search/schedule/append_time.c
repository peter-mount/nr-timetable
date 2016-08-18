
#include <stdio.h>
#include <stdlib.h>
#include <area51/charbuffer.h>
#include <string.h>

void append_hhmm(CharBuffer *b, char *n, int tm) {
    if (tm >= 0)
        charbuffer_printf(b, ",\"%s\":\"%02d:%02d\"", n, (tm / 3600) % 24, (tm / 60) % 60);
}

void append_hhmmss(CharBuffer *b, char *n, int tm) {
    if (tm >= 0)
        charbuffer_printf(b, ",\"%s\":\"%02d:%02d:%02d\"", n, (tm / 3600) % 24, (tm / 60) % 60, tm % 60);
}
