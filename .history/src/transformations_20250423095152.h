#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "cfg_reader.h"

void simplifyCFG();

void groupProductions();

void performLeftFactoring();

void removeDirectLeftRecursion();

#endif // TRANSFORMATIONS_H
