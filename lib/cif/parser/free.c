#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <area51/hashmap.h>
#include <area51/cif.h>

void cif_freeParser(struct CIFParser *p) {
    if(p->handlers)
        hashmapFree(p->handlers);
}
