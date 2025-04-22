#ifndef PARSE_TABLE_H
#define PARSE_TABLE_H

#include "LL1_parser.h"

#define MAX_SYMBOL_LEN 20
#define MAX_SYMBOLS 100
#define MAX_PRODUCTIONS 50

// Structure to represent the LL(1) parsing table
typedef struct {
    char nonTerminal[MAX_SYMBOL_LEN];
    char terminal[MAX_SYMBOL_LEN];
    Production production;
    bool isError;
} ParseTableEntry;

typedef struct {
    ParseTableEntry entries[MAX_SYMBOLS * MAX_SYMBOLS];
    int count;
    char terminals[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    int terminalCount;
    char nonTerminals[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    int nonTerminalCount;
} ParseTable;

// Function declarations
bool isInFirstSet(const char* nonTerminal, const char* terminal);
bool isInFollowSet(const char* nonTerminal, const char* terminal);
void getFirstSetOfSequence(const char* sequence, SymbolSet* result);
ParseTable buildParseTable(Production prods[], int numProds);
void printParseTable(ParseTable table);
void freeParseTable(ParseTable* table);
bool isLL1Grammar(ParseTable table);
void generateAndPrintLL1Table(Production prods[], int numProds);

#endif // PARSE_TABLE_H