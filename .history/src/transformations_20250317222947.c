#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.c"

void performLeftFactoring() {
    for (int i = 0; i < productionCount; i++) {
        char lhs = grammer[i].lhs;
        char *rhs = grammer[i].rhs;

        // finds common prefixes
        for (int j = i + 1; j < productionCount; j++) {
            if (grammer[j].lhs == lhs) {
                char *rhs2 = grammer[j].rhs;
                int k = 0;

                // finds largest common prefix
                while (rhs[k] == rhs2[k] ) {
                    k++;
                }

                // if there is a common prefix
                if (k > 0) {
                    char newNonTerminal = 
                }
            }
        }
    }
}