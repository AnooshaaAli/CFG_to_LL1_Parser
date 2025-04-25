#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "LL1_parser.h"
#include "parse_table.h"

#define MAX_INPUT_LENGTH 1000
#define MAX_STACK_SIZE 100
#define MAX_ERRORS 100 // Maximum number of errors we can store

// Stack structure for parsing
typedef struct {
    char symbols[MAX_STACK_SIZE][MAX_SYMBOL_LEN];
    int top;
} ParseStack;

typedef struct {
    int line_num;
    char error[100];
} SyntaxErrors;

// Array to store syntax errors
SyntaxErrors syntaxErrors[MAX_ERRORS];
int errorCount = 0;

// Initialize the stack
void initStack(ParseStack* stack) {
    stack->top = -1;
}

// Push a symbol onto the stack
bool push(ParseStack* stack, const char* symbol) {
    if (stack->top >= MAX_STACK_SIZE - 1) {
        printf("Error: Stack overflow\n");
        return false;
    }
    stack->top++;
    strcpy(stack->symbols[stack->top], symbol);
    return true;
}

// Pop a symbol from the stack
bool pop(ParseStack* stack, char* symbol) {
    if (stack->top < 0) {
        printf("Error: Stack underflow\n");
        return false;
    }
    strcpy(symbol, stack->symbols[stack->top]);
    stack->top--;
    return true;
}

// Print stack contents
void printStack(ParseStack* stack) {
    printf("[");
    for (int i = 0; i <= stack->top; i++) {
        printf("%s", stack->symbols[i]);
        if (i < stack->top) printf(" ");
    }
    printf("]");
}

// Read input string from file
bool readInputString(const char* filename, char* input, int maxLength) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open input file %s\n", filename);
        return false;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    input[0] = '\0';

    while ((read = getline(&line, &len, file)) != -1) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        // Trim leading/trailing spaces
        char* start = line;
        while (*start == ' ') start++;
        char* end = start + strlen(start) - 1;
        while (end > start && *end == ' ') *end-- = '\0';
        
        if (strlen(start) > 0) {
            if (strlen(input) + strlen(start) + 1 < maxLength) {
                if (input[0] != '\0') strcat(input, " ");
                strcat(input, start);
            } else {
                printf("Error: Input string too long\n");
                free(line);
                fclose(file);
                return false;
            }
        }
    }

    free(line);
    fclose(file);
    return true;
}

bool matchesTerminal(const char* terminal, const char* token) {
    const char* explicitTerminals[] = {
        "if", "else", "return", "int", "float", "char", "+", "-", "*", "/", "=", ";", "(", ")", "{", "}", "$"
    };
    int terminalCount = sizeof(explicitTerminals) / sizeof(explicitTerminals[0]);

    for (int i = 0; i < terminalCount; i++) {
        if (strcmp(token, explicitTerminals[i]) == 0) {
            return strcmp(terminal, token) == 0; 
        }
    }
    
    if (strcmp(terminal, "id") == 0) {
        return isalpha(token[0]);
    } else if (strcmp(terminal, "num") == 0) {
        for (int i = 0; token[i] != '\0'; i++) {
            if (!isdigit(token[i])) return false;
        }
        return true;
    }

    return strcmp(terminal, token) == 0;
}

// Add error to the syntax error list
void addError(int line_num, const char* error_message) {
    if (errorCount < MAX_ERRORS) {
        syntaxErrors[errorCount].line_num = line_num;
        strcpy(syntaxErrors[errorCount].error, error_message);
        errorCount++;
    }
}

// Parse the input string using the LL(1) parsing table
void parseString(ParseTable* table, const char* input, const char* startSymbol) {
    ParseStack stack;
    initStack(&stack);

    // Push end marker and start symbol
    push(&stack, "$");
    push(&stack, startSymbol);

    // Tokenize input string
    char inputCopy[MAX_INPUT_LENGTH];
    strcpy(inputCopy, input);
    char* tokens[MAX_SYMBOLS];
    int tokenCount = 0;

    char* token = strtok(inputCopy, " ");
    while (token != NULL) {
        tokens[tokenCount++] = token;
        token = strtok(NULL, " ");
    }
    tokens[tokenCount] = "$"; // Add end marker
    tokenCount++;

    int inputIndex = 0;
    bool error = false;

    printf("\nPARSING STEPS:\n");
    printf("----------------------------------------------------------------\n");
    printf("%-20s %-15s %-30s\n", "Stack", "Input", "Action");
    printf("----------------------------------------------------------------\n");

    char last_error_input[MAX_INPUT_LENGTH] = "";
    char new_error_input[MAX_INPUT_LENGTH] = "";

    while (stack.top >= 0 && !error) {

        // Print current state
        printf("%s", "");
        printStack(&stack);
        printf(" %-15s", "");
        printf(" %-25s", tokens[inputIndex]);

        char stackTop[MAX_SYMBOL_LEN];
        if (!pop(&stack, stackTop)) {
            error = true;
            break;
        }

        // Case 1: Stack top is a terminal
        if (isTerminal(stackTop) && strcmp(stackTop, "$") != 0) {
            if (matchesTerminal(stackTop, tokens[inputIndex])) {
                printf("%-30s\n", "Match and advance");
                inputIndex++;
            } else {
                strcpy(new_error_input, tokens[inputIndex]);
                if (strcmp(new_error_input, last_error_input) != 0) {
                    strcpy(last_error_input, new_error_input);
                    char errorMsg[100];
                    snprintf(errorMsg, sizeof(errorMsg), "Error: Terminal mismatch. Expected %s, found %s", stackTop, tokens[inputIndex]);
                    addError(inputIndex + 1, errorMsg); // Store error with line number (just a placeholder for this example)
                }
                printf("%-30s\n", "Error: Terminal mismatch");
            }
        }

        // Case 2: Stack top is a non-terminal
        else if (isNonTerminal(stackTop)) {
            // Find table entry
            bool found = false;
            ParseTableEntry* entry = NULL;
            for (int i = 0; i < table->count; i++) {
                if (strcmp(table->entries[i].nonTerminal, stackTop) == 0 &&
                    matchesTerminal(table->entries[i].terminal, tokens[inputIndex])) {
                    entry = &table->entries[i];
                    found = true;
                    break;
                }
            }

            if (found && !entry->isError) {
                if (strcmp(entry->production.rhs, "ε") == 0) {
                    printf("%-30s\n", "Apply ε-production");
                } else {
                    char action[100];
                    snprintf(action, sizeof(action), "Apply %s -> %s", entry->production.lhs, entry->production.rhs);
                    printf("%-30s\n", action);

                    // Push RHS symbols in reverse order
                    char rhsSymbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];
                    int rhsSymbolCount = 0;
                    parseRHS(entry->production.rhs, rhsSymbols, &rhsSymbolCount);
                    for (int i = rhsSymbolCount - 1; i >= 0; i--) {
                        push(&stack, rhsSymbols[i]);
                    }
                }
            } else {
                strcpy(new_error_input, tokens[inputIndex]);
                if (strcmp(new_error_input, last_error_input) != 0) {
                    strcpy(last_error_input, new_error_input);

                    // Adding the specific error message
                    char errorMsg[100];
                    char expectedTerminal[MAX_SYMBOL_LEN];

                    if (entry != NULL) {
                        sscanf(entry->production.rhs, "%s", expectedTerminal);
                    } else {
                        strcpy(expectedTerminal, "unknown");
                    }

                    snprintf(errorMsg, sizeof(errorMsg), 
                            "Error: Expected token [%s], but encountered [%s]", 
                            expectedTerminal, tokens[inputIndex]);
                    addError(inputIndex + 1, errorMsg); // Store error with line number (just a placeholder for this example)
                }
                printf("%-30s\n", "No table entry");
            }
        }

        // Case 3: Stack top is $
        else if (strcmp(stackTop, "$") == 0 && strcmp(tokens[inputIndex], "$") == 0) {
            printf("%-30s\n", "Accept");
            break;
        }
        else {
            strcpy(new_error_input, tokens[inputIndex]);
            if (strcmp(new_error_input, last_error_input) != 0) {
                strcpy(last_error_input, new_error_input);
                char errorMsg[100];
                snprintf(errorMsg, sizeof(errorMsg), "Error: Invalid stack symbol %s", stackTop);
                addError(inputIndex + 1, errorMsg); // Store error with line number (just a placeholder for this example)
            }
            printf("%-30s\n", "Error: Invalid stack symbol");
        }
    }

    printf("----------------------------------------------------------------\n");
    if (!error && stack.top == -1 && inputIndex == tokenCount - 1) {
        printf("\nParsing completed with %d errors!\n", errorCount);
    } else {
        printf("\nParsing failed.\n");
    }

    // Print all collected errors
    if (errorCount > 0) {
        printf("\nSyntax Errors:\n");
        for (int i = 0; i < errorCount; i++) {
            printf("Line %d: %s\n", syntaxErrors[i].line_num, syntaxErrors[i].error);
        }
    }
}

// Main function to parse a string from a file
void parseStringFromFile(ParseTable* table, const char* filename, const char* startSymbol) {
    char input[MAX_INPUT_LENGTH];
    if (readInputString(filename, input, MAX_INPUT_LENGTH)) {
        printf("\nStart Symbol: %s\n", startSymbol);
        printf("\nInput string: %s\n", input);
        parseString(table, input, startSymbol);
    }
}
