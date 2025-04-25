#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.h"

Production *grammar = NULL;
int productionCount = 0;
int maxProductions = 100;

void trimTrailingWhitespace(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

void trimLeadingWhitespace(char **str) {
    while (**str == ' ' || **str == '\t') {
        (*str)++;
    }
}

void readCFGFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    grammar = (Production *)malloc(maxProductions * sizeof(Production));
    if (!grammar) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        if (read <= 1) continue;

        // Expand memory if needed
        if (productionCount >= maxProductions) {
            maxProductions *= 2;
            grammar = realloc(grammar, maxProductions * sizeof(Production));
            if (!grammar) {
                printf("Reallocation failed!\n");
                exit(1);
            }
        }

        // Find "->"
        char *arrow = strstr(line, "->");
        if (!arrow) {
            printf("Invalid production (missing '->'): %s", line);
            continue;
        }

        *arrow = '\0';  // split the string
        char *lhs = line;
        char *rhs = arrow + 2;

        trimLeadingWhitespace(&lhs);
        trimLeadingWhitespace(&rhs);
        trimTrailingWhitespace(lhs);
        trimTrailingWhitespace(rhs);

        // Store LHS
        strncpy(grammar[productionCount].lhs, lhs, sizeof(grammar[productionCount].lhs) - 1);
        grammar[productionCount].lhs[sizeof(grammar[productionCount].lhs) - 1] = '\0';

        // Store RHS
        grammar[productionCount].rhs = malloc(strlen(rhs) + 1);
        if (!grammar[productionCount].rhs) {
            printf("Memory allocation failed for RHS!\n");
            exit(1);
        }
        strcpy(grammar[productionCount].rhs, rhs);

        productionCount++;
    }

    free(line);
    fclose(file);
}

void printGrammer() {
    for (int i = 0; i < productionCount; i++) {
        printf("%s -> %s\n", grammar[i].lhs, grammar[i].rhs);
    }
}

void free_memory() {
    for (int i = 0; i < productionCount; i++) {
        free(grammar[i].rhs);
    }
    free(grammar);
    grammar = NULL;
    productionCount = 0;
}
