#include "../include/diff.h"

errorStatus_e FindDerivative(const char *inFileName)
{
    $INFO("starting diff function");
    errorStatus_e statusCode = OK;

    FILE *inFile = stdin;
    if (inFileName) {
        inFile = fopen(inFileName, "r");
        if (!inFile)
            return $ERR(WRIN, "NO FILE MATCHING USER INPUT NAME %s", inFileName);
    }

    binTree_t inTree  = {};
    string_t  instr   = {};

    bool varsExistList[VARS_AMOUNT] = {};

    RUN_COMMAND(ReadDataAndCreateTree(&inTree, &instr, inFile, varsExistList), "PROBLEM WHILE BUILDING TREE");

    binTree_t outTree = {};
    vars_e varToDiff = {};
    RUN_COMMAND(GetVarToDiff(varsExistList, &varToDiff), "PROBLEM WHILE GETTING DIFF VAR");

    RUN_COMMAND(Diff(inTree.data, &outTree.data, varToDiff), "PROBLEM WHILE TRYING TO DERIVATE");

    TreeDump(&outTree);

    $INFO("ending diff function");
    return statusCode;
}

errorStatus_e Diff(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);


    switch (IN_TYPE)
    {
    case CONST_T: return DiffConst(outNodePtr);
    case VAR_T  : return DiffVar  (IN_VAR, outNodePtr, varToDiff);
    case OP_T   : return OK;
    default:
        return $ERR(WRIN, "UNSUPPORTED NODE TYPE WILE TRYING TO DIFF");
    }
}

errorStatus_e DiffConst(binTreeNode_t **outNodePtr)
{
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_CONST(0);

    return statusCode;
}

errorStatus_e DiffVar(vars_e var, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    if (var == varToDiff) {
        INIT_CONST(1);
    }
    else {
        INIT_CONST(0);
    }
    
    return statusCode;
}

errorStatus_e GetVarToDiff(bool *varsExistList, vars_e *resVar)
{
    assert(varsExistList);
    assert(resVar);

    char var = 0;

    printf("По какой переменной хочешь дифференцировать: ");
    scanf("%c", &var);
    ClearBuffer();

    int ind = 0;
    for (; ind < VARS_AMOUNT; ind++) {
        if (VARS[ind].name == var) {
            *resVar = VARS[ind].varCode;
            break;
        }
    } 

    if (ind == VARS_AMOUNT) {
        printf(RED "NO SUCH VAR", COLOR_RESET);
        return $ERR(WRIN, "USER INPUT VAR <%c> IS NOT IN VAR LIST", var);
    }

    if (!varsExistList[ind])
        printf(YEL "Предупреждаю: переменной которую вы ввели нету в выражении\n" COLOR_RESET);
    
    return OK;
}