#include <stdio.h>
#include <stdlib.h>
#include <area51/cif.h>

/**
 * Copy string from CIF to a buffer.
 * 
 * The buffer must be len+1 bytes long.
 * 
 * @param src Source CIF line
 * @param offset Offset in line
 * @param dst Destination
 * @param len Length
 */
int cif_readString(char *src, int offset, char *dst, int len) {
    for (int i = 0; i < len; i++)
        dst[i] = src[offset + i];
    dst[len] = 0;

    // Truncate the string of trailing spaces
    for (int i = len - 1; i>-1 && dst[i] == ' '; i--)
        dst[i] = 0;

    return offset + len;
}