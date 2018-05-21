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
    char *k = (char*) malloc(size+1);
    fread(k, 1, size, f);
    k[size] = '\0';
    fclose(f);
    return k;
}
