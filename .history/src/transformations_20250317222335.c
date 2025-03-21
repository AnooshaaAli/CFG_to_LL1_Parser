#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.c"

void performLeftFactoring() {
    for (int i = 0; i < productionCount; i++) {
        char lhs = grammer[i].lhs;
        char *rhs = grammer[i].rhs;

        // finds common prefixes
        for (int j = i + 1; j < productionCount; j++) {
            
        }
    }
}