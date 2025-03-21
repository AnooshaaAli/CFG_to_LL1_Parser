#include <stdio.h>
#include <stdlib.h>  // Add this to define NULL
#include <string.h>
#include "cfg_reader.h"

Production *grammar = NULL;
int productionCount = 0;
int maxProductions = 100;

// reads cfg from file
void readCfGfromFile(const char* filename) {

    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    grammar = (Production*)malloc(maxProductions * sizeof(Production));

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {

        // skips empty lines
        if (read <= 1) {
            continue;
        }

        // incrreases production count
        if (productionCount >= maxProductions) {
            maxProductions *= 2;
            grammar = (Production*)realloc(grammar, maxProductions * sizeof(Production));
        }

        char *lineCopy = malloc(strlen(line));
        strcpy(lineCopy, line);

        char *lhsToken = strtok(lineCopy, " ->");
        if (lhsToken) {
            strcpy(grammar[productionCount].lhs, lhsToken);
        }

        // skips spaces and arrow
        char *arrow = strstr(line, "->");
        char *rhsStart = arrow + 2;

        if (rhsStart >= line + read) {
            printf("Error: Invalid RHS in production: %s\n", line);
            continue;
        }

        while (*rhsStart == ' ' && rhsStart < line + read) {
            rhsStart++;
        }

        grammar[productionCount].rhs = malloc(strlen(rhsStart) + 1);
        if (!grammar[productionCount].rhs) {
            printf("Memory allocation failed!\n");
            exit(1);
        }

        strcpy(grammar[productionCount].rhs, rhsStart);
        grammar[productionCount].rhs[strcspn(grammar[productionCount].rhs, "\n")] = '\0';
        productionCount++;
    }
    fclose(file);
}

void simplifyCFG() {
    for (int i = 0; i < productionCount; i++) {
        
    }
}
// print the grammer
void printGrammer() {
    for (int i = 0; i < productionCount; i++) {
        printf("%s -> %s\n", grammar[i].lhs, grammar[i].rhs);
    }
}

// free memory
void free_memory() {
    for (int i = 0; i < productionCount; i++) {
        free(grammar[i].rhs);
    }
    free(grammar);
}