#ifndef CFG_READER_H
#define CFG_READER_H

#include <stdio.h>
#include <stdlib.h>

// Define struct
typedef struct {
    char *lhs;
    char *rhs;
} Production;

// Declare global variables
extern Production *grammer;
extern int productionCount;

// Function declarations
void readCFGFromFile(const char *filename);
void printGrammar();
void free_memory();

#endif // CFG_READER_H
