#include <stdio.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <area51/cif.h>

int cif_parseFile(struct CIFParser *parser, int fsock) {
    struct stat sb;

    if (fsock == -1 || fstat(fsock, &sb) == -1)
        return EXIT_FAILURE;

    void *fmap = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fsock, 0);
    if (fmap == MAP_FAILED)
        return EXIT_FAILURE;

    int rc = cif_parse(parser, fmap, sb.st_size);

    munmap(fmap, sb.st_size);
    return rc;
}