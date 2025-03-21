#include <stdio.h>
#include <stdlib.h>

// reads cfg from file
void readCfGfromFile(const char* filename) {
    
    FILE *file = fopen(filename, 'r');

    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }


}

int main() {
    char* filename;
    readCFGfromFile(filename);
    return 0;
}
