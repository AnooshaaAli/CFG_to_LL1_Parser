#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.c"
#include "transformations.c"

// main code
int main() {
    char* filename = "../input/cfg.txt";
    readCfGfromFile(filename);
    sim
    printGrammer();
    performLeftFactoring();
    printGrammer();
    free_memory();
    return 0;
}
