#ifndef STRING_PARSER_H
#define STRING_PARSER_H

#include "parse_table.h"

void parseStringFromFile(ParseTable* table, const char* filename, const char* startSymbol);

#endif // STRING_PARSER_H