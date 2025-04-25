#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transformations.h"
#include "LL1_parser.h"
#include "parse_table.h"
#include "string_parser.h"

// main code
int main() {
    char* cfgFilename = "../input/test_input.txt";
    char* stringFilename = "../input/test_string.txt";

    printf("\nInitial CFG: \n");
    readCFGFromFile(cfgFilename);
    printGrammer();

    printf("\nCFG after left factoring: \n");
    performLeftFactoring();
    printGrammer();

    printf("\nCFG after removing left recursion: \n");
    removeDirectLeftRecursion(); 
    removeIndirectLeftRecursion();
    printGrammer();

    int newProductionCount;
    Production* correctedGrammar = correctProductionFormat(grammar, productionCount, &newProductionCount);

    // Generate LL(1) parsing table
    extractSymbols(correctedGrammar, newProductionCount);
    computeFirstSets(correctedGrammar, newProductionCount);
    computeFollowSets(correctedGrammar, newProductionCount);
    printSets(firstSets, numFirstSets, "FIRST");
    printSets(followSets, numFollowSets, "FOLLOW");

    ParseTable table = buildParseTable(correctedGrammar, newProductionCount);
    printParseTable(table);

    bool ll1Status = isLL1Grammar(table);
    printf("\nThe grammar is %sLL(1).\n", ll1Status ? "" : "NOT ");

    // Parse input string
    if (ll1Status) {
        parseStringFromFile(&table, stringFilename, correctedGrammar[0].lhs);
    } else {
        printf("\nCannot parse string: Grammar is not LL(1).\n");
    }

    // Free memory
    freeParseTable(&table);
    for (int i = 0; i < newProductionCount; i++) {
        free(correctedGrammar[i].rhs);
    }
    free(correctedGrammar);
    free_memory();
    
    return 0;
}