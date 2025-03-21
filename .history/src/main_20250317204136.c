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

    FILE *file = fopen(filename, 'r');

    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    grammer = (Production*)malloc(maxProductions * sizeof(Production));

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {

        // skips empty lines
        if (line[0] != '\n') {
            continue;
        }

        if (productionCount >= maxProductions) {
            maxProductions *= 2;
            grammer = (Production*)realloc(grammer, maxProductions * sizeof(Production))
        }
    }
}

int main() {
    char* filename;
    readCFGfromFile(filename);
    return 0;
}
