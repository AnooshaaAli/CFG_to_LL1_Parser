#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

// Include the cfg_reader.h which contains the Production struct and global variables
#include "cfg_reader.h"

// Simplifies the CFG by merging productions with the same LHS
void simplifyCFG();

// Groups similar productions by their common prefixes
void groupProductions();

// Performs left factoring on the grammar to eliminate common prefixes
void performLeftFactoring();

// Removes direct left recursion from the grammar
void removeDirectLeftRecursion();

#endif // TRANSFORMATIONS_H
