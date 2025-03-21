#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.h"

void performLeftFactoring() {
    for (int i = 0; i < productionCount; i++) {
        char *lhs = grammar[i].lhs;
        char *rhs = grammar[i].rhs;

        // finds common prefixes
        for (int j = i + 1; j < productionCount; j++) {
            if (strcmp(grammar[j].lhs, lhs) == 0) {  
                char *rhs2 = grammar[j].rhs;
                int k = 0;

                // finds largest common prefix
                while (rhs[k] == rhs2[k] ) {
                    k++;
                }

                // if there is a common prefix
                if (k > 0) {

                    char *newNonTerminal = malloc(strlen(lhs) + 1);
                    snprintf(newNonTerminal, strlen(lhs) + 2, "%s'", lhs);
                    char *newRhs = malloc(k + 1);
                    strncpy(newRhs, rhs, k);
                    newRhs[k] = '\0';

                    char *newProduction1 = strdup(rhs + k);
                    char *newProduction2 = strdup(rhs2 + k);

                    snprintf(grammar[i].rhs, strlen(newRhs) + strlen(newNonTerminal) + 2, "%s%s", newRhs, newNonTerminal);
                    strcpy(grammar[j].lhs, newNonTerminal);
                    snprintf(grammar[j].rhs, strlen(newProduction1) + strlen(newProduction2) + 4, "%s | %s", newProduction1, newProduction2);

                    free(newRhs);
                    free(newProduction1);
                    free(newProduction2);

                }
            }
        }
    }
}