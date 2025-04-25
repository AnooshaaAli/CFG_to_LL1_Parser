#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfg_reader.h"

#define MAX_RULE_LENGTH 100
#define INITIAL_PRODUCTIONS 10

/*
void performLeftFactoring() {
    simplifyCFG();
    for (int i = 0; i < productionCount; i++) {
        char *lhs = grammar[i].lhs;
        char *rhs = grammar[i].rhs;

        // simplify a prod rule
        char *tokens[100];
        int tokenCount = 0;

        char *rhsCopy = strdup(rhs);
        char *token = strtok(rhsCopy, "|");

        while (token != NULL) {

            // trim leading spaces
            while (*token == ' ') token++;

            // trim trailing spaces
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') {
                *end = '\0'; 
                end--;
            }
            
            //printf("%s\n", token);
            tokens[tokenCount] = strdup(token);  
            tokenCount++;
            token = strtok(NULL, "|");
        }

        free(rhsCopy);

        // find length of longest common prefix
        int prefixLength = strlen(tokens[0]);
        for (int j = 1; j < tokenCount; j++) {
            int k = 0;
            while (tokens[j][k] && tokens[0][k] && tokens[j][k] == tokens[0][k]) {
                k++;
            }
            prefixLength = (k < prefixLength) ? k : prefixLength;
        }

        if (prefixLength > 0) {

            // create new production
            char newLHS[10];
            strcpy(newLHS, lhs);
            strcat(newLHS, "'");
            char *newRhs = malloc(prefixLength + strlen(newLHS) + 2);
            strncpy(newRhs, tokens[0], prefixLength);
            newRhs[prefixLength] = '\0';
            strcat(newRhs, newLHS);
            grammar[i].rhs = newRhs;

            // add new production
            Production newProduction;
            strcpy(newProduction.lhs, newLHS);
            newProduction.rhs = malloc(strlen(rhs));
            newProduction.rhs[0] = '\0'; 

            for (int j = 0; j < tokenCount; j++) {
                //printf("%s\n", tokens[j] + prefixLength);
                strcat(newProduction.rhs, tokens[j] + prefixLength);
                if (j < tokenCount - 1) strcat(newProduction.rhs, " | ");
            }

            grammar[productionCount] = newProduction;
            productionCount++;
        }

        for (int j = 0; j < tokenCount; j++) {
            free(tokens[j]);
        }
    }
}

*/

void simplifyCFG() {
    Production *newGrammar = (Production *)malloc(maxProductions * sizeof(Production));
    int newCount = 0;

    int *merged = (int *)calloc(productionCount, sizeof(int));

    for (int i = 0; i < productionCount; i++) {
        if (merged[i]) continue; 

        char *lhs = grammar[i].lhs;

        char mergedRHS[1000] = "";  
        strcat(mergedRHS, grammar[i].rhs);  

        for (int j = i + 1; j < productionCount; j++) {
            if (strcmp(lhs, grammar[j].lhs) == 0) { 
                strcat(mergedRHS, " | ");  
                strcat(mergedRHS, grammar[j].rhs);
                merged[j] = 1; 
            }
        }

        strcpy(newGrammar[newCount].lhs, lhs);
        newGrammar[newCount].rhs = malloc(strlen(mergedRHS) + 1);
        strcpy(newGrammar[newCount].rhs, mergedRHS);
        newCount++;
    }

    free(grammar);
    grammar = newGrammar;
    productionCount = newCount;
    free(merged);
}

void groupProductions() {
    for (int i = 0; i < productionCount; i++) {
        char *tokens[100];
        int tokenCount = 0;

        char *lhs = grammar[i].lhs;
        char *rhs =  grammar[i].rhs;

        char *rhsCopy = strdup(rhs);
        char *token = strtok(rhsCopy, "|");

        while (token != NULL) {

            // trim leading spaces
            while (*token == ' ') token++;

            // trim trailing spaces
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') {
                *end = '\0'; 
                end--;
            }
            
            //printf("%s\n", token);
            tokens[tokenCount] = strdup(token);  
            tokenCount++;
            token = strtok(NULL, "|");
        }

        free(rhsCopy);

        Production *Groups = (Production *)malloc(tokenCount * sizeof(Production));
        int groupCount = 0;

        for (int j = 0; j < tokenCount; j++) {
            char *token = tokens[j];
            int found = 0;

            // match token with existing groups
            for (int k = 0; k < groupCount; k++) {
                int commonPrefixLen = 0;
                while (Groups[k].rhs[commonPrefixLen] && token[commonPrefixLen] &&
                       Groups[k].rhs[commonPrefixLen] == token[commonPrefixLen]) {
                    commonPrefixLen++;
                }

                if (commonPrefixLen >= 1) { 
                    size_t newSize = strlen(Groups[k].rhs) + strlen(token) + 4;
                    Groups[k].rhs = (char *)realloc(Groups[k].rhs, newSize);
                    strcat(Groups[k].rhs, " | ");
                    strcat(Groups[k].rhs, token);
                    found = 1;
                    break;
                }
            }

            if (!found) {
                strcpy(Groups[groupCount].lhs, lhs); 
                Groups[groupCount].rhs = strdup(token);
                groupCount++;
            }
        }

        for (int j = 0; j < tokenCount; j++) {
            free(tokens[j]);
        }

        free(grammar[i].rhs); 
        grammar[i].rhs = strdup(Groups[0].rhs);

        for (int j = 1; j < groupCount; j++) {
            strcpy(grammar[productionCount].lhs, Groups[j].lhs);
            grammar[productionCount].rhs = strdup(Groups[j].rhs);
            productionCount++;
        }

        for (int j = 0; j < groupCount; j++) {
            free(Groups[j].rhs);
        }
        free(Groups);
    }
}

void performLeftFactoring() {
    simplifyCFG();
    groupProductions();
    //printGrammer();

    int newNonTerminalCount = 0; 
    for (int i = 0; i < productionCount; i++) {
        char *lhs = grammar[i].lhs;
        char *rhs = grammar[i].rhs;

        // simplify a prod rule
        char *tokens[100];
        int tokenCount = 0;

        char *rhsCopy = strdup(rhs);
        char *token = strtok(rhsCopy, "|");

        while (token != NULL) {

            // trim leading spaces
            while (*token == ' ') token++;

            // trim trailing spaces
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') {
                *end = '\0'; 
                end--;
            }
            
            //printf("%s\n", token);
            tokens[tokenCount] = strdup(token);  
            tokenCount++;
            token = strtok(NULL, "|");
        }

        free(rhsCopy);

        // find length of longest common prefix
        int prefixLength = strlen(tokens[0]);
        for (int j = 1; j < tokenCount; j++) {
            int k = 0;
            while (tokens[j][k] && tokens[0][k] && tokens[j][k] == tokens[0][k]) {
                k++;
            }

            // handles cases like (S -> id | id + = )
            if(k > 1 && !tokens[j][k] || !tokens[0][k])
                k++;

            prefixLength = (k < prefixLength) ? k : prefixLength;
        }

        if (prefixLength > 0 && tokenCount > 1) {

            // create new production
            char newLHS[100];
            snprintf(newLHS, sizeof(newLHS), "%s%d", lhs, ++newNonTerminalCount);
            char *newRhs = malloc(prefixLength + strlen(newLHS) + 2);
            strncpy(newRhs, tokens[0], prefixLength);
            newRhs[prefixLength] = '\0';
            strcat(newRhs, newLHS);
            grammar[i].rhs = newRhs;

            // add new production
            Production newProduction;
            strcpy(newProduction.lhs, newLHS);
            newProduction.rhs = malloc(strlen(rhs));
            newProduction.rhs[0] = '\0'; 

            for (int j = 0; j < tokenCount; j++) {
                //printf("%s\n", tokens[j] + prefixLength);
                strcat(newProduction.rhs, tokens[j] + prefixLength);
                if (j < tokenCount - 1) strcat(newProduction.rhs, " | ");
            }

            grammar[productionCount] = newProduction;
            productionCount++;
        }

        for (int j = 0; j < tokenCount; j++) {
            free(tokens[j]);
        }
    }
    simplifyCFG();
}

void removeDirectLeftRecursion() {
    simplifyCFG();
    int newNonTerminalCount = 0; 

    for (int i = 0; i < productionCount; i++) {
        char *lhs = grammar[i].lhs;
        char *rhs = grammar[i].rhs;

        char *tokens[100];
        int tokenCount = 0;

        char *rhsCopy = strdup(rhs);
        char *token = strtok(rhsCopy, "|");

        while (token != NULL) {
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') {
                *end = '\0';
                end--;
            }

            tokens[tokenCount] = strdup(token);
            tokenCount++;
            token = strtok(NULL, "|");
        }

        free(rhsCopy);

        int hasLeftRecursion = 0;
        char *nonRecursive[100];
        char *recursive[100];
        int nonRecCount = 0, recCount = 0;

        // separate left-recursive and non-recursive rules
        for (int j = 0; j < tokenCount; j++) {
            if (strncmp(tokens[j], lhs, strlen(lhs)) == 0) {
                hasLeftRecursion = 1;
                char *temp = tokens[j] + strlen(lhs);
                while (*temp == ' ') temp++;  
                recursive[recCount++] = strdup(temp);
 
            } else {
                nonRecursive[nonRecCount++] = tokens[j];
            }
        }

        if (hasLeftRecursion) {
            char newLHS[100];
            snprintf(newLHS, sizeof(newLHS), "%s'", lhs);

            char *newRhs = malloc(100);
            newRhs[0] = '\0';

            if (nonRecCount == 0) {
                // case where no β exists (A -> Aα only)
                strcat(newRhs, newLHS);
            } else {
                for (int j = 0; j < nonRecCount; j++) {
                    strcat(newRhs, nonRecursive[j]);
                    strcat(newRhs, " ");
                    strcat(newRhs, newLHS);
                    if (j < nonRecCount - 1) strcat(newRhs, " | ");
                }
            }

            grammar[i].rhs = newRhs;

            Production newProduction;
            strcpy(newProduction.lhs, newLHS);
            newProduction.rhs = malloc(100);
            newProduction.rhs[0] = '\0';

            for (int j = 0; j < recCount; j++) {
                strcat(newProduction.rhs, recursive[j]);
                strcat(newProduction.rhs, " ");
                strcat(newProduction.rhs, newLHS);
                if (j < recCount - 1) strcat(newProduction.rhs, " | ");
            }

            if (nonRecCount) {
                strcat(newProduction.rhs, " | ε");
            }

            grammar[productionCount++] = newProduction;
        }

        for (int j = 0; j < tokenCount; j++) {
            free(tokens[j]);
        }
    }
}

void removeIndirectLeftRecursion() {
    for (int i = 0; i < productionCount; i++) {
        for (int j = 0; j < i; j++) {
            if (strcmp(grammar[i].lhs, grammar[j].lhs) == 0) 
                continue;

            char *tokens[100];
            int tokenCount = 0;

            char *rhsCopy = strdup(grammar[i].rhs);
            char *token = strtok(rhsCopy, "|");

            while (token) {
                while (*token == ' ') token++; 
                char *end = token + strlen(token) - 1;
                while (end > token && *end == ' ') {
                    *end = '\0';
                    end--;
                }
                tokens[tokenCount++] = strdup(token);
                token = strtok(NULL, "|");
            }
            free(rhsCopy);

            char updatedRHS[1000] = "";

            for (int k = 0; k < tokenCount; k++) {
                if (strncmp(tokens[k], grammar[j].lhs, strlen(grammar[j].lhs)) == 0 &&
                    (tokens[k][strlen(grammar[j].lhs)] == ' ' || tokens[k][strlen(grammar[j].lhs)] == '\0')) {

                    char *suffix = tokens[k] + strlen(grammar[j].lhs);
                    while (*suffix == ' ') suffix++;  

                    char *rhs_j_copy = strdup(grammar[j].rhs);
                    char *subToken = strtok(rhs_j_copy, "|");

                    while (subToken) {
                        while (*subToken == ' ') subToken++; 
                        char *end = subToken + strlen(subToken) - 1;
                        while (end > subToken && *end == ' ') {
                            *end = '\0';
                            end--;
                        }

                        strcat(updatedRHS, subToken);
                        if (strlen(suffix) > 0) {
                            strcat(updatedRHS, " ");
                            strcat(updatedRHS, suffix);
                        }
                        strcat(updatedRHS, " | ");

                        subToken = strtok(NULL, "|");
                    }
                    free(rhs_j_copy);
                } else {
                    strcat(updatedRHS, tokens[k]);
                    strcat(updatedRHS, " | ");
                }
            }

            int len = strlen(updatedRHS);
            if (len >= 3) updatedRHS[len - 3] = '\0';

            free(grammar[i].rhs);
            grammar[i].rhs = strdup(updatedRHS);

            for (int k = 0; k < tokenCount; k++) {
                free(tokens[k]);
            }
        }
        removeDirectLeftRecursion();
    }
}
