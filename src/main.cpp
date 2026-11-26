#include "../include/diff.h"
#include "../include/calc.h"

#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    InitLogDir();
    FindDerivative(0);

    return OK;
}