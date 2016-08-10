#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/hashmap.h>
#include <area51/cif.h>

int cif_initParser(struct CIFParser *p) {
    memset(p, 0, sizeof (struct CIFParser));
    p->handlers = hashmapCreate(100, hashmapStringHash, hashmapStringEquals);
    if(!p->handlers) {
        cif_freeParser(p);
        return 1;
    }
    return 0;
}
