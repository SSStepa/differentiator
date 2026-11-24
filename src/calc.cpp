#include "../include/calc.h"

int main()
{

    InitLogDir();
    binTree_t tree = {};
    string_t  str  = {};

    if (ReadDataAndCreateTree(&tree, &str, stdin) != OK) {
        printf(RED "PROBLEM BRO");
    }

    return 0;
}
