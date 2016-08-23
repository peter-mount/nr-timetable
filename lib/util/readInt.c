#include <stdio.h>
#include <stdlib.h>
#include <area51/cif.h>

int cif_readInt(char *src, int offset, int len) {
    int v = INT_MIN;

    if (len > 0) {
        char tmp[len + 1];
        cif_readString(src, offset, tmp, len);
        v = atoi(tmp);
    }

    return v;
}

int cif_readInt_r(char *src, int offset, int *dst, int len) {
    *dst = cif_readInt(src, offset, len);
    return offset + len;
}
