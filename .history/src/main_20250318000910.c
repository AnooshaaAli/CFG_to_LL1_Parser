#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.c"
#include "transformations.c"

// main code
int main() {
    char* filename = "../input/cfg.txt";
    readCfGfromFile(filename);

    // write this func
    //simplifyCFG();
    printf("\nInitial CFG: \n");
    printGrammer();

    printf("\nCFG after left factoring: \n");
    performLeftFactoring();
    printGrammer();
    free_memory();
    return 0;
}
