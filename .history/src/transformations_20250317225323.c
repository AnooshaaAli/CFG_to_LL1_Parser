#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.h"

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
                    char *newRhs = malloc(k + 1);
                    strncpy(newRhs, rhs, k);
                    newRhs[k] = '\0';

                    char *newProduction1 = strdup(rhs + k);
                    char *newProduction2 = strdup(rhs2 + k);

                    snprintf(grammer[i].rhs, strlen(newRhs) + 2, "%s%c", newRhs, newNonTerminal);
                    grammer[j].lhs = newNonTerminal;
                    snprintf(grammer[j].rhs, strlen(newProduction1) + strlen(newProduction2) + 4, "%s | %s", newProduction1, newProduction2);

                    free(newRhs);
                    free(newProduction1);
                    free(newProduction2);

                }
            }
        }
    }
}