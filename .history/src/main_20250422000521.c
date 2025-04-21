#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LL1_parsing.c"
#include "parse_table.c"

// main code
int main() {
    char* filename = "../cfg/cfg.txt.txt";

    printf("\nInitial CFG: \n");
    readCfGfromFile(filename);
    printGrammer();

    printf("\nCFG after left factoring: \n");
    performLeftFactoring();
    printGrammer();

    printf("\nCFG after remvoving left recusrsion: \n");
    removeDirectLeftRecursion(); 
    removeIndirectLeftRecursion();
    printGrammer();

    int newProductionCount;
    Production* correctedGrammar = correctProductionFormat(grammar, productionCount, &newProductionCount);

    generateAndPrintLL1Table(correctedGrammar, newProductionCount);

    printf("\n");
    free_memory();
    
    return 0;
}
