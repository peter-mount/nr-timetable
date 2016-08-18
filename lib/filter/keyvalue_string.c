
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <area51/hashmap.h>

/**
 * Filter that accepts a KeyValue and matches a string against the key
 */
bool filter_keyvalue_string(void *v, void *c) {
    KeyValue *e = v;
    char *uid = c;
    return strcmp((char*) e->key, uid) == 0;
}
