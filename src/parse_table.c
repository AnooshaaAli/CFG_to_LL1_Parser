#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "parse_table.h"

// Check if terminal exists in first set of a non-terminal
bool isInFirstSet(const char* nonTerminal, const char* terminal) {
    int index = getFirstSetIndex(nonTerminal);
    if (index != -1) {
        for (int i = 0; i < firstSets[index].count; i++) {
            if (strcmp(firstSets[index].elements[i].symbol, terminal) == 0) {
                return true;
            }
        }
    }
    return false;
}

// Check if terminal exists in follow set of a non-terminal
bool isInFollowSet(const char* nonTerminal, const char* terminal) {
    int index = getFollowSetIndex(nonTerminal);
    if (index != -1) {
        for (int i = 0; i < followSets[index].count; i++) {
            if (strcmp(followSets[index].elements[i].symbol, terminal) == 0) {
                return true;
            }
        }
    }
    return false;
}

// Get first set of a sequence of symbols
void getFirstSetOfSequence(const char* sequence, SymbolSet* result) {
    // Parse sequence into individual symbols
    char symbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    int symbolCount = 0;
    parseRHS(sequence, symbols, &symbolCount);
    
    // Clear result
    result->count = 0;
    
    bool allCanDeriveEpsilon = true;
    
    for (int i = 0; i < symbolCount && allCanDeriveEpsilon; i++) {
        if (isTerminal(symbols[i])) {
            // If it's a terminal, add it to result and we're done
            strcpy(result->elements[result->count].symbol, symbols[i]);
            result->elements[result->count].exists = true;
            result->count++;
            allCanDeriveEpsilon = false;
        } else {
            // For non-terminals, add all elements from its FIRST set except epsilon
            int firstSetIndex = getFirstSetIndex(symbols[i]);
            if (firstSetIndex != -1) {
                for (int j = 0; j < firstSets[firstSetIndex].count; j++) {
                    const char* terminal = firstSets[firstSetIndex].elements[j].symbol;
                    if (strcmp(terminal, "ε") != 0) {
                        // Check if terminal is already in result
                        bool alreadyExists = false;
                        for (int k = 0; k < result->count; k++) {
                            if (strcmp(result->elements[k].symbol, terminal) == 0) {
                                alreadyExists = true;
                                break;
                            }
                        }
                        
                        if (!alreadyExists) {
                            strcpy(result->elements[result->count].symbol, terminal);
                            result->elements[result->count].exists = true;
                            result->count++;
                        }
                    }
                }
                
                // Check if this symbol can derive epsilon
                allCanDeriveEpsilon = hasEpsilonInFirstSet(symbols[i]);
            } else {
                allCanDeriveEpsilon = false;
            }
        }
    }
    
    // If all symbols can derive epsilon, add epsilon to result
    if (allCanDeriveEpsilon && symbolCount > 0) {
        strcpy(result->elements[result->count].symbol, "ε");
        result->elements[result->count].exists = true;
        result->count++;
    } else if (symbolCount == 0) {
        // Empty sequence produces epsilon
        strcpy(result->elements[result->count].symbol, "ε");
        result->elements[result->count].exists = true;
        result->count++;
    }
}

// Build the LL(1) parsing table
ParseTable buildParseTable(Production prods[], int numProds) {
    ParseTable table;
    table.count = 0;
    table.terminalCount = 0;
    table.nonTerminalCount = 0;
    
    // Extract and store all terminals (including $) and non-terminals
    for (int i = 0; i < numSymbols; i++) {
        if (isTerminal(allSymbols[i]) && strcmp(allSymbols[i], "ε") != 0) {
            strcpy(table.terminals[table.terminalCount], allSymbols[i]);
            table.terminalCount++;
        } else if (isNonTerminal(allSymbols[i])) {
            strcpy(table.nonTerminals[table.nonTerminalCount], allSymbols[i]);
            table.nonTerminalCount++;
        }
    }
    
    // Add the end marker if not already present
    bool endMarkerFound = false;
    for (int i = 0; i < table.terminalCount; i++) {
        if (strcmp(table.terminals[i], "$") == 0) {
            endMarkerFound = true;
            break;
        }
    }
    
    if (!endMarkerFound) {
        strcpy(table.terminals[table.terminalCount], "$");
        table.terminalCount++;
    }
    
    // Initialize table with error entries
    for (int i = 0; i < table.nonTerminalCount; i++) {
        for (int j = 0; j < table.terminalCount; j++) {
            ParseTableEntry entry;
            strcpy(entry.nonTerminal, table.nonTerminals[i]);
            strcpy(entry.terminal, table.terminals[j]);
            strcpy(entry.production.lhs, "");
            entry.production.rhs = NULL;
            entry.isError = true;
            
            table.entries[table.count] = entry;
            table.count++;
        }
    }
    
    // Fill the table based on the algorithm
    for (int i = 0; i < numProds; i++) {
        const char* lhs = prods[i].lhs;
        const char* rhs = prods[i].rhs;
        
        // Case 1: A -> α (where α is not ε)
        if (strcmp(rhs, "ε") != 0) {
            // Calculate FIRST(α)
            SymbolSet firstOfRHS;
            strcpy(firstOfRHS.symbol, "FIRST_OF_RHS");
            getFirstSetOfSequence(rhs, &firstOfRHS);
            
            // For each terminal a in FIRST(α), add A -> α to table[A, a]
            for (int j = 0; j < firstOfRHS.count; j++) {
                const char* terminal = firstOfRHS.elements[j].symbol;
                if (strcmp(terminal, "ε") != 0) {
                    // Find the table entry for this non-terminal and terminal
                    for (int k = 0; k < table.count; k++) {
                        if (strcmp(table.entries[k].nonTerminal, lhs) == 0 && 
                            strcmp(table.entries[k].terminal, terminal) == 0) {
                            
                            // Check for conflicts (if entry is already filled)
                            if (!table.entries[k].isError) {
                                printf("LL(1) conflict: Multiple entries for [%s, %s]\n", lhs, terminal);
                                printf("Existing: %s -> %s\n", table.entries[k].production.lhs, table.entries[k].production.rhs);
                                printf("New: %s -> %s\n", lhs, rhs);
                            }
                            
                            // Set the production
                            strcpy(table.entries[k].production.lhs, lhs);
                            table.entries[k].production.rhs = strdup(rhs);
                            table.entries[k].isError = false;
                            break;
                        }
                    }
                }
            }
            
            // If ε is in FIRST(α), for each terminal b in FOLLOW(A), add A -> α to table[A, b]
            bool epsilonInFirst = false;
            for (int j = 0; j < firstOfRHS.count; j++) {
                if (strcmp(firstOfRHS.elements[j].symbol, "ε") == 0) {
                    epsilonInFirst = true;
                    break;
                }
            }
            
            if (epsilonInFirst) {
                int followSetIndex = getFollowSetIndex(lhs);
                if (followSetIndex != -1) {
                    for (int j = 0; j < followSets[followSetIndex].count; j++) {
                        const char* terminal = followSets[followSetIndex].elements[j].symbol;
                        
                        // Find the table entry for this non-terminal and terminal
                        for (int k = 0; k < table.count; k++) {
                            if (strcmp(table.entries[k].nonTerminal, lhs) == 0 && 
                                strcmp(table.entries[k].terminal, terminal) == 0) {
                                
                                // Check for conflicts
                                if (!table.entries[k].isError) {
                                    printf("LL(1) conflict: Multiple entries for [%s, %s]\n", lhs, terminal);
                                    printf("Existing: %s -> %s\n", table.entries[k].production.lhs, table.entries[k].production.rhs);
                                    printf("New: %s -> %s\n", lhs, rhs);
                                }
                                
                                // Set the production
                                strcpy(table.entries[k].production.lhs, lhs);
                                table.entries[k].production.rhs = strdup(rhs);
                                table.entries[k].isError = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
        // Case 2: A -> ε
        else {
            // For each terminal b in FOLLOW(A), add A -> ε to table[A, b]
            int followSetIndex = getFollowSetIndex(lhs);
            if (followSetIndex != -1) {
                for (int j = 0; j < followSets[followSetIndex].count; j++) {
                    const char* terminal = followSets[followSetIndex].elements[j].symbol;
                    
                    // Find the table entry for this non-terminal and terminal
                    for (int k = 0; k < table.count; k++) {
                        if (strcmp(table.entries[k].nonTerminal, lhs) == 0 && 
                            strcmp(table.entries[k].terminal, terminal) == 0) {
                            
                            // Check for conflicts
                            if (!table.entries[k].isError) {
                                printf("LL(1) conflict: Multiple entries for [%s, %s]\n", lhs, terminal);
                                printf("Existing: %s -> %s\n", table.entries[k].production.lhs, table.entries[k].production.rhs);
                                printf("New: %s -> ε\n", lhs);
                            }
                            
                            // Set the production
                            strcpy(table.entries[k].production.lhs, lhs);
                            table.entries[k].production.rhs = strdup("ε");
                            table.entries[k].isError = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return table;
}

// Print the LL(1) parsing table
void printParseTable(ParseTable table) {
    printf("\nLL(1) PARSING TABLE:\n");
    printf("-------------------\n\n");
    
    // Print header row with terminals
    printf("%-10s |", "");
    for (int i = 0; i < table.terminalCount; i++) {
        printf(" %-15s |", table.terminals[i]);
    }
    printf("\n");
    
    // Print separator
    for (int i = 0; i < 12 + 18 * table.terminalCount; i++) {
        printf("-");
    }
    printf("\n");
    
    // Print rows for each non-terminal
    for (int i = 0; i < table.nonTerminalCount; i++) {
        printf("%-10s |", table.nonTerminals[i]);
        
        for (int j = 0; j < table.terminalCount; j++) {
            bool found = false;
            
            for (int k = 0; k < table.count; k++) {
                if (strcmp(table.entries[k].nonTerminal, table.nonTerminals[i]) == 0 && 
                    strcmp(table.entries[k].terminal, table.terminals[j]) == 0) {
                    
                    if (!table.entries[k].isError) {
                        // Try to make the display fit nicely
                        char shortProd[16]; // Limit display length
                        snprintf(shortProd, 15, "%s -> %s", 
                                table.entries[k].production.lhs, 
                                table.entries[k].production.rhs);
                        printf(" %-15s |", shortProd);
                    } else {
                        printf(" %-15s |", "");
                    }
                    
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                printf(" %-15s |", "");
            }
        }
        
        printf("\n");
    }
    
    printf("-------------------\n");
}

// Free the memory used by parse table
void freeParseTable(ParseTable* table) {
    for (int i = 0; i < table->count; i++) {
        if (!table->entries[i].isError && table->entries[i].production.rhs != NULL) {
            free(table->entries[i].production.rhs);
        }
    }
}

// Function to check if the grammar is LL(1)
bool isLL1Grammar(ParseTable table) {
    bool isLL1 = true;
    
    for (int i = 0; i < table.nonTerminalCount; i++) {
        for (int j = 0; j < table.terminalCount; j++) {
            int conflictCount = 0;
            Production conflictingProductions[MAX_PRODUCTIONS];
            
            // Count entries for this cell
            for (int k = 0; k < table.count; k++) {
                if (strcmp(table.entries[k].nonTerminal, table.nonTerminals[i]) == 0 && 
                    strcmp(table.entries[k].terminal, table.terminals[j]) == 0 && 
                    !table.entries[k].isError) {
                    
                    strcpy(conflictingProductions[conflictCount].lhs, table.entries[k].production.lhs);
                    conflictingProductions[conflictCount].rhs = strdup(table.entries[k].production.rhs);
                    conflictCount++;
                }
            }
            
            // If more than one entry, we have a conflict
            if (conflictCount > 1) {
                isLL1 = false;
                printf("\nConflict in table entry [%s, %s]:\n", table.nonTerminals[i], table.terminals[j]);
                for (int k = 0; k < conflictCount; k++) {
                    printf("  %s -> %s\n", conflictingProductions[k].lhs, conflictingProductions[k].rhs);
                    free(conflictingProductions[k].rhs);
                }
            }
        }
    }
    
    return isLL1;
}

// Main function to generate and print the LL(1) parsing table
void generateAndPrintLL1Table(Production prods[], int numProds) {
    // Extract all symbols from the grammar
    extractSymbols(prods, numProds);
    
    // Compute FIRST and FOLLOW sets
    computeFirstSets(prods, numProds);
    computeFollowSets(prods, numProds);
    
    // Print the FIRST and FOLLOW sets
    printSets(firstSets, numFirstSets, "FIRST");
    printSets(followSets, numFollowSets, "FOLLOW");
    
    // Build and print the LL(1) parsing table
    ParseTable table = buildParseTable(prods, numProds);
    printParseTable(table);
    
    // Check if the grammar is LL(1)
    bool ll1Status = isLL1Grammar(table);
    printf("\nThe grammar is %sLL(1).\n", ll1Status ? "" : "NOT ");
    
    // Free memory
    freeParseTable(&table);
}