#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

// Include the cfg_reader.h which contains the Production struct and global variables
#include "cfg_reader.h"

void simplifyCFG();

void groupProductions();

void performLeftFactoring();

void removeDirectLeftRecursion();

#endif // TRANSFORMATIONS_H
