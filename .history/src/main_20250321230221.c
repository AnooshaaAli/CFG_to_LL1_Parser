#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.c"
#include "transformations.c"

// main code
int main() {
    char* filename = "../input/left_recur_test.txt";

    printf("\nInitial CFG: \n");
    readCfGfromFile(filename);
    printGrammer();

    printf("\nCFG after left factoring: \n");
    performLeftFactoring();
    printGrammer();

    printf("\nCFG after remvoving left recusrsion: \n");
    removeDirectLeftRecursion(); 
    removetLeftRecursion();
    printGrammer();

    printf("\n");
    free_memory();
    
    return 0;
}
