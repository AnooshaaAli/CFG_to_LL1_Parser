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

        sscanf(line, "%s ->", grammar[productionCount].lhs); 

        // skips spaces and arrow
        char *arrow = strstr(line, "->");
        char *rhsStart = arrow + 2;
        while (*rhsStart == ' ') {
            rhsStart++;
        }

        grammar[productionCount].rhs = malloc(strlen(rhsStart) + 1);
        strcpy(grammar[productionCount].rhs, rhsStart);
        grammar[productionCount].rhs[strcspn(grammer[productionCount].rhs, "\n")] = '\0';
        productionCount++;
    }
    fclose(file);
}

// print the grammer
void printGrammer() {
    for (int i = 0; i < productionCount; i++) {
        printf("%s -> %s\n", grammer[i].lhs, grammer[i].rhs);
    }
}

// free memory
void free_memory() {
    for (int i = 0; i < productionCount; i++) {
        free(grammer[i].rhs);
    }
    free(grammer);
}