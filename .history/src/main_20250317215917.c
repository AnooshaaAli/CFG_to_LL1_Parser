#include <stdio.h>
#include <stdlib.h>

#define MAX_LENGTH 100

typedef struct {
    char lhs;
    char *rhs;
} Production;

Production *grammer = NULL;
int productionCount = 0;
int maxProductions = 100;

// reads cfg from file
void readCfGfromFile(const char* filename) {

    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    grammer = (Production*)malloc(maxProductions * sizeof(Production));

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, ))) {

        // skips empty lines
        if (line[0] == '\n') {
            continue;
        }

        // incrreases production count
        if (productionCount >= maxProductions) {
            maxProductions *= 2;
            grammer = (Production*)realloc(grammer, maxProductions * sizeof(Production));
        }

        grammer[productionCount].lhs = line[0];
        grammer[productionCount].rhs = (char*)malloc(100 * sizeof(char));

        // copies rhs after ->
        int j = 0, i = 4;
        while (line[i] != '\n' && line[i] != '\0') {
            grammer[productionCount].rhs[j++] = line[i++];
        }
        grammer[productionCount].rhs[j] = '\0';
        productionCount++;
    }
    fclose(file);
}

// print the grammer
void printGrammer() {
    for (int i = 0; i < productionCount; i++) {
        printf("%c -> %s\n", grammer[i].lhs, grammer[i].rhs);
    }
}

// free memory
void free_memory() {
    for (int i = 0; i < productionCount; i++) {
        free(grammer[i].rhs);
    }
    free(grammer);
}

// main code
int main() {
    char* filename = "../input/cfg.txt";
    readCfGfromFile(filename);
    printGrammer();
    free_memory();
    return 0;
}
