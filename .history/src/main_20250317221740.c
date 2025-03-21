#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <
// main code
int main() {
    char* filename = "../input/cfg.txt";
    readCfGfromFile(filename);
    printGrammer();
    free_memory();
    return 0;
}
