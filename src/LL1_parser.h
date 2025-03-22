#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "cfg_reader.c"
#include "transformations.c"
#include "cfg_reader.h"

#define MAX_SYMBOL_LEN 20
#define MAX_SYMBOLS 100
#define MAX_PRODUCTIONS 50

// Structure to represent sets of terminals and non-terminals
typedef struct {
    char symbol[MAX_SYMBOL_LEN];
    bool exists;
} SetElement;

typedef struct {
    char symbol[MAX_SYMBOL_LEN];
    SetElement elements[MAX_SYMBOLS];
    int count;
} SymbolSet;

// Global variables to store first and follow sets
SymbolSet firstSets[MAX_SYMBOLS];
SymbolSet followSets[MAX_SYMBOLS];
int numFirstSets = 0;
int numFollowSets = 0;

// Global array to store all symbols in the grammar
char allSymbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
int numSymbols = 0;

// Function declarations
void extractSymbols(Production prods[], int numProds);
bool isTerminal(const char* symbol);
bool isNonTerminal(const char* symbol);
void computeFirstSets(Production prods[], int numProds);
void computeFollowSets(Production prods[], int numProds);
void printSets(SymbolSet sets[], int numSets, const char* setName);
int getFirstSetIndex(const char* symbol);
int getFollowSetIndex(const char* symbol);
void initializeFirstSet(const char* symbol);
void initializeFollowSet(const char* symbol);
void addToFirstSet(const char* nonTerminal, const char* terminal);
void addToFollowSet(const char* nonTerminal, const char* terminal);
bool hasEpsilonInFirstSet(const char* symbol);
void copyFirstSetToFollowSet(const char* sourceSymbol, const char* targetSymbol, bool excludeEpsilon);
void copyFollowSetToFollowSet(const char* sourceSymbol, const char* targetSymbol);
void parseRHS(const char* rhs, char symbols[][MAX_SYMBOL_LEN], int* count);
void printCFG(Production prods[], int numProds);
Production* correctProductionFormat(Production* prods, int inputCount, int* outputCount);
void freeProductions(Production* prods, int count);