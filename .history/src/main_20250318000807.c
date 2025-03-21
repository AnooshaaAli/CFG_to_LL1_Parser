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
    
    printGrammer();
    performLeftFactoring();
    printGrammer();
    free_memory();
    return 0;
}
