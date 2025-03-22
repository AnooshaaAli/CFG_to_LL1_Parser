#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "LL1_parser.h"

void printProductionsAsArray(Production *prods, int count) {
    printf("Production prods[] = {\n");
    
    for (int i = 0; i < count; i++) {
        printf("        {\"%s\", \"%s\"}", prods[i].lhs, prods[i].rhs);
        
        // Add comma for all but the last element
        if (i < count - 1) {
            printf(",");
        }
        
        printf("\n");
    }
    
    printf("    };\n");
}

// Function to split productions that have alternatives (using |)
Production* correctProductionFormat(Production* prods, int inputCount, int* outputCount) {
    // First pass: count how many new productions we'll have
    int newCount = 0;
    for (int i = 0; i < inputCount; i++) {
        char* ptr = prods[i].rhs;
        int alternativeCount = 1; // At least one production
        
        // Count occurrences of " | " to determine number of alternatives
        while ((ptr = strstr(ptr, " | ")) != NULL) {
            alternativeCount++;
            ptr += 3; // Move past " | "
        }
        
        newCount += alternativeCount;
    }
    
    // Allocate memory for the new productions
    Production* newProds = (Production*)malloc(newCount * sizeof(Production));
    if (!newProds) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    // Second pass: create the new productions
    int newIndex = 0;
    for (int i = 0; i < inputCount; i++) {
        // If there's no "|" in this production, just copy it directly
        if (strstr(prods[i].rhs, " | ") == NULL) {
            strcpy(newProds[newIndex].lhs, prods[i].lhs);
            newProds[newIndex].rhs = strdup(prods[i].rhs);
            if (!newProds[newIndex].rhs) {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            newIndex++;
            continue;
        }
        
        // If we're here, this production has alternatives to split
        char* rhsCopy = strdup(prods[i].rhs);
        if (!rhsCopy) {
            printf("Memory allocation failed!\n");
            exit(1);
        }
        
        // Split by " | "
        char* token = strtok(rhsCopy, "|");
        while (token != NULL) {
            // Copy the left-hand side
            strcpy(newProds[newIndex].lhs, prods[i].lhs);
            
            // Trim leading and trailing spaces from the token
            char* trimmed = token;
            while (*trimmed == ' ' && *trimmed != '\0') {
                trimmed++;
            }
            
            // Remove trailing spaces
            char* end = trimmed + strlen(trimmed) - 1;
            while (end > trimmed && *end == ' ') {
                *end-- = '\0';
            }
            
            // Allocate memory for the right-hand side
            newProds[newIndex].rhs = strdup(trimmed);
            if (!newProds[newIndex].rhs) {
                printf("Memory allocation failed!\n");
                exit(1);
            }
            
            newIndex++;
            token = strtok(NULL, "|");
        }
        
        free(rhsCopy);
    }
    
    // Update the output count
    *outputCount = newCount;
    
    return newProds;
}

// Function to print the Context-Free Grammar
void printCFG(Production prods[], int numProds) {
    printf("\nCONTEXT-FREE GRAMMAR:\n");
    printf("---------------------\n");
    
    // Keep track of which non-terminals we've already printed
    char printed[MAX_SYMBOLS][MAX_SYMBOL_LEN];
    int numPrinted = 0;
    
    for (int i = 0; i < numProds; i++) {
        // Check if this LHS has been printed before
        bool alreadyPrinted = false;
        for (int j = 0; j < numPrinted; j++) {
            if (strcmp(printed[j], prods[i].lhs) == 0) {
                alreadyPrinted = true;
                break;
            }
        }
        
        // If this is the first production for this non-terminal
        if (!alreadyPrinted) {
            printf("%s -> %s", prods[i].lhs, prods[i].rhs);
            
            // Add to printed list
            strcpy(printed[numPrinted], prods[i].lhs);
            numPrinted++;
            
            // Check for alternative productions with the same LHS
            for (int j = i + 1; j < numProds; j++) {
                if (strcmp(prods[j].lhs, prods[i].lhs) == 0) {
                    printf(" | %s", prods[j].rhs);
                }
            }
            printf("\n");
        }
    }
    printf("---------------------\n");
}

// Extract all symbols from the grammar
void extractSymbols(Production prods[], int numProds) {
    for (int i = 0; i < numProds; i++) {
        // Add LHS symbol if not already added
        bool found = false;
        for (int j = 0; j < numSymbols; j++) {
            if (strcmp(allSymbols[j], prods[i].lhs) == 0) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            strcpy(allSymbols[numSymbols], prods[i].lhs);
            numSymbols++;
        }
        
        // Parse RHS and add symbols
        char rhsSymbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
        int rhsSymbolCount = 0;
        parseRHS(prods[i].rhs, rhsSymbols, &rhsSymbolCount);
        
        for (int j = 0; j < rhsSymbolCount; j++) {
            found = false;
            for (int k = 0; k < numSymbols; k++) {
                if (strcmp(allSymbols[k], rhsSymbols[j]) == 0) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                strcpy(allSymbols[numSymbols], rhsSymbols[j]);
                numSymbols++;
            }
        }
    }
    
    // Add epsilon symbol if not already added
    bool epsilonFound = false;
    for (int i = 0; i < numSymbols; i++) {
        if (strcmp(allSymbols[i], "ε") == 0) {
            epsilonFound = true;
            break;
        }
    }
    
    if (!epsilonFound) {
        strcpy(allSymbols[numSymbols], "ε");
        numSymbols++;
    }
}

// Parse RHS of a production into individual symbols
void parseRHS(const char* rhs, char symbols[][MAX_SYMBOL_LEN], int* count) {
    *count = 0;
    char temp[MAX_SYMBOL_LEN];
    int tempIndex = 0;
    
    for (int i = 0; rhs[i] != '\0'; i++) {
        if (rhs[i] == ' ' || rhs[i] == '\t' || rhs[i] == '|') {
            if (tempIndex > 0) {
                temp[tempIndex] = '\0';
                strcpy(symbols[*count], temp);
                (*count)++;
                tempIndex = 0;
            }
        } else {
            temp[tempIndex++] = rhs[i];
        }
    }
    
    if (tempIndex > 0) {
        temp[tempIndex] = '\0';
        strcpy(symbols[*count], temp);
        (*count)++;
    }
}

// Check if a symbol is a terminal
bool isTerminal(const char* symbol) {
    // Check for special cases first
    if (strcmp(symbol, "ε") == 0 || 
        strcmp(symbol, "+") == 0 || 
        strcmp(symbol, "*") == 0 || 
        strcmp(symbol, "(") == 0 || 
        strcmp(symbol, ")") == 0 || 
        strcmp(symbol, "=") == 0 || 
        strcmp(symbol, ";") == 0 || 
        strcmp(symbol, "id") == 0 ||
        strcmp(symbol, "$") == 0) {
        return true;
    }
    
    // If the first character is a single quote or if the symbol contains a prime,
    // it might be a non-terminal with a prime notation
    if (strchr(symbol, '\'') != NULL) {
        return false;
    }
    
    // Otherwise, assume non-terminals have capitalized first letters
    return !isupper(symbol[0]);
}

// Check if a symbol is a non-terminal
bool isNonTerminal(const char* symbol) {
    return !isTerminal(symbol);
}

// Get the index of the first set for a symbol
int getFirstSetIndex(const char* symbol) {
    for (int i = 0; i < numFirstSets; i++) {
        if (strcmp(firstSets[i].symbol, symbol) == 0) {
            return i;
        }
    }
    return -1;
}

// Get the index of the follow set for a symbol
int getFollowSetIndex(const char* symbol) {
    for (int i = 0; i < numFollowSets; i++) {
        if (strcmp(followSets[i].symbol, symbol) == 0) {
            return i;
        }
    }
    return -1;
}

// Initialize a first set for a symbol
void initializeFirstSet(const char* symbol) {
    int index = getFirstSetIndex(symbol);
    if (index == -1) {
        strcpy(firstSets[numFirstSets].symbol, symbol);
        firstSets[numFirstSets].count = 0;
        numFirstSets++;
    }
}

// Initialize a follow set for a symbol
void initializeFollowSet(const char* symbol) {
    int index = getFollowSetIndex(symbol);
    if (index == -1 && isNonTerminal(symbol)) {
        strcpy(followSets[numFollowSets].symbol, symbol);
        followSets[numFollowSets].count = 0;
        numFollowSets++;
    }
}

// Add a terminal to the first set of a non-terminal
void addToFirstSet(const char* nonTerminal, const char* terminal) {
    int index = getFirstSetIndex(nonTerminal);
    if (index != -1) {
        for (int i = 0; i < firstSets[index].count; i++) {
            if (strcmp(firstSets[index].elements[i].symbol, terminal) == 0) {
                return; // Element already exists
            }
        }
        strcpy(firstSets[index].elements[firstSets[index].count].symbol, terminal);
        firstSets[index].elements[firstSets[index].count].exists = true;
        firstSets[index].count++;
    }
}

// Add a terminal to the follow set of a non-terminal
void addToFollowSet(const char* nonTerminal, const char* terminal) {
    int index = getFollowSetIndex(nonTerminal);
    if (terminal == "|") {
        return;
    }
    if (index != -1) {
        for (int i = 0; i < followSets[index].count; i++) {
            if (strcmp(followSets[index].elements[i].symbol, terminal) == 0) {
                return; // Element already exists
            }
        }
        strcpy(followSets[index].elements[followSets[index].count].symbol, terminal);
        followSets[index].elements[followSets[index].count].exists = true;
        followSets[index].count++;
    }
}

// Check if a symbol has epsilon in its first set
bool hasEpsilonInFirstSet(const char* symbol) {
    int index = getFirstSetIndex(symbol);
    if (index != -1) {
        for (int i = 0; i < firstSets[index].count; i++) {
            if (strcmp(firstSets[index].elements[i].symbol, "ε") == 0) {
                return true;
            }
        }
    }
    return false;
}

// Copy first set of one symbol to the follow set of another symbol
void copyFirstSetToFollowSet(const char* sourceSymbol, const char* targetSymbol, bool excludeEpsilon) {
    int sourceIndex = getFirstSetIndex(sourceSymbol);
    if (sourceIndex != -1) {
        for (int i = 0; i < firstSets[sourceIndex].count; i++) {
            const char* terminal = firstSets[sourceIndex].elements[i].symbol;
            if (!excludeEpsilon || strcmp(terminal, "ε") != 0) {
                addToFollowSet(targetSymbol, terminal);
            }
        }
    }
}

// Copy follow set of one symbol to the follow set of another symbol
void copyFollowSetToFollowSet(const char* sourceSymbol, const char* targetSymbol) {
    int sourceIndex = getFollowSetIndex(sourceSymbol);
    if (sourceIndex != -1) {
        for (int i = 0; i < followSets[sourceIndex].count; i++) {
            const char* terminal = followSets[sourceIndex].elements[i].symbol;
            addToFollowSet(targetSymbol, terminal);
        }
    }
}

// Compute first sets for all symbols in the grammar
void computeFirstSets(Production prods[], int numProds) {
    // Initialize first sets for all symbols
    for (int i = 0; i < numSymbols; i++) {
        initializeFirstSet(allSymbols[i]);
        
        // If symbol is a terminal, add it to its own first set
        if (isTerminal(allSymbols[i])) {
            addToFirstSet(allSymbols[i], allSymbols[i]);
        }
    }
    
    bool changed;
    do {
        changed = false;
        
        for (int i = 0; i < numProds; i++) {
            const char* lhs = prods[i].lhs;
            const char* rhs = prods[i].rhs;
            
            // If production is of the form X -> ε
            if (strcmp(rhs, "ε") == 0) {
                int beforeCount = firstSets[getFirstSetIndex(lhs)].count;
                addToFirstSet(lhs, "ε");
                if (beforeCount != firstSets[getFirstSetIndex(lhs)].count) {
                    changed = true;
                }
                continue;
            }
            
            // Parse RHS into individual symbols
            char rhsSymbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
            int rhsSymbolCount = 0;
            parseRHS(rhs, rhsSymbols, &rhsSymbolCount);
            
            // For productions X -> Y1 Y2 ... Yn
            bool allCanDeriveEpsilon = true;
            for (int j = 0; j < rhsSymbolCount && allCanDeriveEpsilon; j++) {
                if (isTerminal(rhsSymbols[j])) {
                    int beforeCount = firstSets[getFirstSetIndex(lhs)].count;
                    addToFirstSet(lhs, rhsSymbols[j]);
                    if (beforeCount != firstSets[getFirstSetIndex(lhs)].count) {
                        changed = true;
                    }
                    allCanDeriveEpsilon = false;
                } else if (isNonTerminal(rhsSymbols[j])) {
                    int firstSetIndex = getFirstSetIndex(rhsSymbols[j]);
                    if (firstSetIndex != -1) {
                        int beforeCount = firstSets[getFirstSetIndex(lhs)].count;
                        
                        // Add all terminals except epsilon from First(Y) to First(X)
                        for (int k = 0; k < firstSets[firstSetIndex].count; k++) {
                            const char* terminal = firstSets[firstSetIndex].elements[k].symbol;
                            if (strcmp(terminal, "ε") != 0) {
                                addToFirstSet(lhs, terminal);
                            }
                        }
                        
                        if (beforeCount != firstSets[getFirstSetIndex(lhs)].count) {
                            changed = true;
                        }
                        
                        // Check if Y can derive epsilon
                        allCanDeriveEpsilon = hasEpsilonInFirstSet(rhsSymbols[j]);
                    } else {
                        allCanDeriveEpsilon = false;
                    }
                }
            }
            
            // If all symbols in RHS can derive epsilon, add epsilon to First(X)
            if (allCanDeriveEpsilon && rhsSymbolCount > 0) {
                int beforeCount = firstSets[getFirstSetIndex(lhs)].count;
                addToFirstSet(lhs, "ε");
                if (beforeCount != firstSets[getFirstSetIndex(lhs)].count) {
                    changed = true;
                }
            }
        }
    } while (changed);
}

// Compute follow sets for all non-terminals in the grammar
void computeFollowSets(Production prods[], int numProds) {
    // Initialize follow sets for all non-terminals
    for (int i = 0; i < numSymbols; i++) {
        if (isNonTerminal(allSymbols[i])) {
            initializeFollowSet(allSymbols[i]);
        }
    }
    
    // Add $ to Follow(S) where S is the start symbol (first LHS in grammar)
    addToFollowSet(prods[0].lhs, "$");
    
    bool changed;
    do {
        changed = false;
        
        for (int i = 0; i < numProds; i++) {
            const char* lhs = prods[i].lhs;
            const char* rhs = prods[i].rhs;
            
            // Skip epsilon productions
            if (strcmp(rhs, "ε") == 0) {
                continue;
            }
            
            // Parse RHS into individual symbols
            char rhsSymbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
            int rhsSymbolCount = 0;
            parseRHS(rhs, rhsSymbols, &rhsSymbolCount);
            
            for (int j = 0; j < rhsSymbolCount; j++) {
                if (isNonTerminal(rhsSymbols[j])) {
                    int followSetIndex = getFollowSetIndex(rhsSymbols[j]);
                    if (followSetIndex == -1) continue;
                    
                    int beforeCount = followSets[followSetIndex].count;
                    
                    // If A -> αBβ, add First(β) - {ε} to Follow(B)
                    if (j < rhsSymbolCount - 1) {
                        bool allNextCanDeriveEpsilon = true;
                        
                        // Process the next symbols (β)
                        for (int k = j + 1; k < rhsSymbolCount && allNextCanDeriveEpsilon; k++) {
                            if (isTerminal(rhsSymbols[k])) {
                                addToFollowSet(rhsSymbols[j], rhsSymbols[k]);
                                allNextCanDeriveEpsilon = false;
                            } else if (isNonTerminal(rhsSymbols[k])) {
                                copyFirstSetToFollowSet(rhsSymbols[k], rhsSymbols[j], true);
                                
                                // Check if this non-terminal can derive epsilon
                                allNextCanDeriveEpsilon = hasEpsilonInFirstSet(rhsSymbols[k]);
                            }
                        }
                        
                        // If all remaining symbols can derive epsilon, add Follow(A) to Follow(B)
                        if (allNextCanDeriveEpsilon) {
                            copyFollowSetToFollowSet(lhs, rhsSymbols[j]);
                        }
                    } 
                    // If A -> αB, add Follow(A) to Follow(B)
                    else {
                        copyFollowSetToFollowSet(lhs, rhsSymbols[j]);
                    }
                    
                    if (beforeCount != followSets[followSetIndex].count) {
                        changed = true;
                    }
                }
            }
        }
    } while (changed);
}

// Print the sets
void printSets(SymbolSet sets[], int numSets, const char* setName) {
    printf("\n%s SETS:\n", setName);
    for (int i = 0; i < numSets; i++) {
        // Print only for non-terminals (and epsilon for first sets)
        if (isNonTerminal(sets[i].symbol) || 
            (strcmp(setName, "FIRST") == 0 && strcmp(sets[i].symbol, "ε") == 0)) {
            
            printf("%s(%s) = { ", setName, sets[i].symbol);
            if (sets[i].count == 0) {
                printf("empty set");
            } else {
                for (int j = 0; j < sets[i].count; j++) {
                    printf("%s", sets[i].elements[j].symbol);
                    if (j < sets[i].count - 1) {
                        printf(", ");
                    }
                }
            }
            printf(" }\n");
        }
    }
}