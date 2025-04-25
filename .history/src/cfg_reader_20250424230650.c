#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.h"

// Define global variables
Production *grammar = NULL;
int productionCount = 0;
int maxProductions = 100;

// Function to trim leading and trailing spaces
void trimSpaces(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;

    // Trim leading spaces
    while (*start == ' ' && start <= end) start++;

    // Trim trailing spaces
    while (*end == ' ' && end >= start) end--;

    // Shift the string to the front
    memmove(str, start, end - start + 1);
    str[end - start + 1] = '\0';
}

// Reads CFG from file
void readCFGFromFile(const char* filename) {
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
        // Skip empty lines
        if (read <= 1) {
            continue;
        }

        // Increase production count
        if (productionCount >= maxProductions) {
            maxProductions *= 2;
            grammar = (Production*)realloc(grammar, maxProductions * sizeof(Production));
        }

        char *lineCopy = malloc(strlen(line) + 1); // +1 for null terminator
        strcpy(lineCopy, line);

        // Tokenize the left-hand side (LHS)
        char *lhsToken = strtok(lineCopy, " ->");
        if (lhsToken) {
            strcpy(grammar[productionCount].lhs, lhsToken);
            trimSpaces(grammar[productionCount].lhs); // Trim spaces for LHS
        }

        // Find the arrow (->) and move the pointer past it to get the RHS
        char *arrow = strstr(line, "->");
        char *rhsStart = arrow + 2;

        if (rhsStart >= line + read) {
            printf("Error: Invalid RHS in production: %s\n", line);
            free(lineCopy);
            continue;
        }

        // Skip spaces before RHS
        while (*rhsStart == ' ' && rhsStart < line + read) {
            rhsStart++;
        }

        // Allocate memory for RHS and copy it
        grammar[productionCount].rhs = malloc(strlen(rhsStart) + 1);
        if (!grammar[productionCount].rhs) {
            printf("Memory allocation failed!\n");
            exit(1);
        }

        strcpy(grammar[productionCount].rhs, rhsStart);
        trimSpaces(grammar[productionCount].rhs); // Trim spaces for RHS
        grammar[productionCount].rhs[strcspn(grammar[productionCount].rhs, "\n")] = '\0'; // Remove newline from RHS

        // Increment production count
        productionCount++;
        free(lineCopy);
    }
    free(line);
    fclose(file);
}

// Print the grammar
void printGrammer() {
    for (int i = 0; i < productionCount; i++) {
        printf("%s -> %s\n", grammar[i].lhs, grammar[i].rhs);
    }
}

// Free memory
void freeemory() {
    for (int i = 0; i < productionCount; i++) {
        free(grammar[i].rhs);
    }
    free(grammar);
    grammar = NULL;
    productionCount = 0;
}

