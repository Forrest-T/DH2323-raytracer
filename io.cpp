#include "stdlib.h"
#include "stdio.h"

#include "io.hpp"

char *readKernel(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Unable to load kernel: %s\n", filename);
        exit(1);
    }
    fseek(f, 0L, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *k = (char*) malloc(size);
    fread(k, 1, size, f);
    fclose(f);
    return k;
}
