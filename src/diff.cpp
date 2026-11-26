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

    TreeDtor(&inTree);
    TreeDtor(&outTree);

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
    case OP_T   : return DiffOp   (inNode,  outNodePtr, varToDiff);
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

errorStatus_e DiffOp(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    RUN_COMMAND(diffFuncList[IN_OP](inNode, outNodePtr, varToDiff), "PROBLEM WHILE FINDING DIFF");

    return statusCode;
}

errorStatus_e DiffOpSum(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);

    errorStatus_e statusCode = OK;

    INIT_OP(OP_ADD);

    DIFF_L(&OUT_L);
    DIFF_R(&OUT_R);
    
    return OK;
}

// (uv)' = u'v + v'u
errorStatus_e DiffOpMult(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_ADD);
    INIT_SUB_OP(OP_MULT, &OUT_L);
    INIT_SUB_OP(OP_MULT, &OUT_R);
    
    DIFF_L(&OUT_LL);
    COPY_R(&OUT_LR);

    DIFF_R(&OUT_RL);
    COPY_L(&OUT_RR);

    return statusCode;
}

errorStatus_e DiffOpSub(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);

    errorStatus_e statusCode = OK;

    INIT_OP(OP_SUB);

    DIFF_L(&OUT_L);
    DIFF_R(&OUT_R);
    
    return OK;

}

// (u/v)' = (u'v - v'u)/(v^2)
errorStatus_e DiffOpDiv (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);

    INIT_SUB_OP(OP_SUB, &OUT_L);
    INIT_SUB_OP(OP_MULT, &OUT_LL);
    DIFF_L(&(OUT_LL -> left ));
    COPY_R(&(OUT_LL -> right));
    INIT_SUB_OP(OP_MULT, &OUT_LR);
    DIFF_R(&(OUT_LR -> left));
    COPY_L(&(OUT_LR -> right));

    INIT_SUB_OP(OP_DEG, &OUT_R);
    COPY_R(&OUT_RL);
    INIT_SUB_CONST(2, &OUT_RR);

    return statusCode;
}

errorStatus_e CopyTreeNode(binTreeNode_t *inNode, binTreeNode_t **outNodePtr)
{
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    if (!inNode) return OK;

    RUN_COMMAND(TreeNodeCreate(inNode -> val, outNodePtr), "PROBLEM WHILE CREATING NODE COPY");
    RUN_COMMAND(CopyTreeNode(IN_L, &OUT_L), "PROBLEM WHILE COPYING LEFT SON");
    RUN_COMMAND(CopyTreeNode(IN_R, &OUT_R), "PROBLEM WHILE COPYING RIGHT SON");

    return OK;
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
        printf(RED "NO SUCH VAR" COLOR_RESET);
        return $ERR(WRIN, "USER INPUT VAR <%c> IS NOT IN VAR LIST", var);
    }

    if (!varsExistList[ind])
        printf(YEL "Предупреждаю: переменной которую вы ввели нету в выражении\n" COLOR_RESET);
    
    return OK;
}