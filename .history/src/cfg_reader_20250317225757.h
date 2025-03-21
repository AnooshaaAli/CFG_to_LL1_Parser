#ifndef CFG_READER_H
#define CFG_READER_H

typedef struct {
    char lhs[3];
    char *rhs;
} Production;

extern Production *grammar;  // Declare as extern
extern int productionCount;

void readCFGFromFile(const char *filename);

#endif // CFG_READER_H
