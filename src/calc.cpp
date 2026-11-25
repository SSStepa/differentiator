#include "../include/calc.h"

int main()
{

    InitLogDir();

    Calc(NULL);

    return 0;
}

errorStatus_e Calc(const char *inFileName)
{
    $INFO("starting calculation process");
    errorStatus_e statusCode = OK;

    FILE *inFile = stdin;
    if (inFileName) {
        inFile = fopen(inFileName, "r");
        if (!inFile)
            return $ERR(WRIN, "NO FILE MATCHING USER INPUT NAME %s", inFileName);
    }
    binTree_t tree = {};
    string_t  str  = {};

    double *varsList = (double *) calloc(VARS_AMOUNT, sizeof(double));
    for (int i = 0; i < VARS_AMOUNT; i++)
        varsList[i] = NAN;

    RUN_COMMAND(ReadDataAndCreateTree(&tree, &str, inFile, varsList), "PROBLEM WHILE CREATING TREE");

    double resVal = NAN;
    RUN_COMMAND(DoCalcOperations(tree.data, &resVal, varsList), "PROBLEM WHILE CALCULATION");
    printf("%lg", resVal);

    $INFO("ending calculation process");
    return statusCode;
}

errorStatus_e DoCalcOperations(binTreeNode_t *node, double *outVal, double* varList)
{
    assert(outVal);
    assert(varList);

    if (!node)
        return OK;

    switch (NODE_T)
    {
    case CONST_T:
        *outVal = NODE_CONST;
        return OK;
    case VAR_T:
        *outVal = varList[NODE_VAR];
        return OK;
    case OP_T:
    {
        double fVal = NAN;
        double sVal = NAN;
        if (DoCalcOperations(node -> left, &fVal, varList) != OK || DoCalcOperations(node -> right, &sVal, varList) != OK)
            return $ERR(WRIN, "PROBLEM WITH FINDING OPERANDS");
        
        switch (NODE_OP)
        {
        case OP_ADD: 
            *outVal = fVal + sVal;
            break;
        case OP_MULT:
            *outVal = fVal * sVal;
            break;
        case OP_SUB:
            *outVal = fVal - sVal;
            break;
        case OP_DIV:
            *outVal = fVal / sVal;
            break;
        case OP_DEG:
            *outVal = pow(fVal, sVal);
            break;
        case OP_LOG:
            *outVal = log(sVal)/log(fVal);
            break;
        case OP_SIN:
            *outVal = sin(sVal);
            break;
        case OP_COS:
            *outVal = cos(sVal);
            break;
        case OP_TAN:
            *outVal = tan(sVal);
            break;
        case OP_COT:
            *outVal = 1.0/tan(sVal);
            break;
        case OP_ASIN:
            *outVal = asin(sVal);
            break;
        case OP_ACOS:
            *outVal = acos(sVal);
            break;
        case OP_ATAN:
            *outVal = atan(sVal);
            break;
        case OP_ACOT:
            *outVal = atan2(1.0, sVal);
            break;
        case OP_SH:
            *outVal = sinh(sVal);
            break;
        case OP_CH:
            *outVal = cosh(sVal);
            break;
        case OP_TH:
            *outVal = tanh(sVal);
            break;
        case OP_CTH:
            *outVal = 1.0/tanh(sVal);
            break;
        case OP_ASH:
            *outVal = asinh(sVal);
            break;
        case OP_ACH:
            *outVal = acosh(sVal);
            break;
        case OP_ATH:
            *outVal = atanh(sVal);
            break;
        case OP_ACTH:
            *outVal = atanh(1.0/sVal);
            break;
        case OP_NULL: default:
            return $ERR(WRIN, "UNSUPPORTED COMMAND");
        }
        return OK;
    }
    default:
        break;
    }
}
