#ifndef CFG_READER_H
#define CFG_READER_H

typedef struct {
    char lhs[100];
    char *rhs;
} Production;

extern Production *grammar;
extern int productionCount;

void readCFGFromFile(const char *filename);
void printGrammer();
void free_memory();

#endif
