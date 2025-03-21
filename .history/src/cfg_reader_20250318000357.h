#ifndef CFG_READER_H
#define CFG_READER_H

typedef struct {
    char lhs[10];
    char *rhs;
} Production;

extern Production *grammar;  
extern int productionCount;

void readCFGFromFile(const char *filename);
void simplifyCFG
#endif // CFG_READER_H
