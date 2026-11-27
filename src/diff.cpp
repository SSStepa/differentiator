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

    bool IsChanged = false;
    do {
        ConstFold(outTree.data);
        AlgebraSimpl(&outTree.data);
    } while (IsChanged);

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
errorStatus_e DiffOpDiv(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        INIT_SUB_OP(OP_SUB, &OUT_L);
            INIT_SUB_OP(OP_MULT, &OUT_LL);
                DIFF_L(&L(OUT_LL));
                COPY_R(&R(OUT_LL));
            INIT_SUB_OP(OP_MULT, &OUT_LR);
                DIFF_R(&L(OUT_LR));
                COPY_L(&R(OUT_LR));
        INIT_SUB_OP(OP_DEG, &OUT_R);
            COPY_R(&OUT_RL);
            INIT_SUB_CONST(2, &OUT_RR);

    return statusCode;
}

// (u^v)' = (u^v)(v'ln(u) + vu'/u) but some special cases: (c^c) = 0 (u^c)' = cu^(c-1)u'  (c^u)' = c^u*lnc*u'
errorStatus_e DiffOpDeg(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    if (T(IN_L) == CONST_T && T(IN_R) == CONST_T) {
        INIT_CONST(0);
    }
    else if (T(IN_R) == CONST_T) {
        INIT_OP(OP_MULT);
            INIT_SUB_OP(OP_MULT, &OUT_L);
                DIFF_L(&OUT_LL);
                INIT_SUB_CONST(C(IN_R), &OUT_LR);
            INIT_SUB_OP(OP_DEG, &OUT_R);
                COPY_L(&OUT_RL);
                INIT_SUB_CONST(C(IN_R) - 1, &OUT_RR);
    }
    else if (T(IN_L) == CONST_T) {
        INIT_OP(OP_MULT);
            INIT_SUB_OP(OP_DEG, &OUT_L);
                INIT_SUB_CONST(C(IN_L), &OUT_LL);
                COPY_R(&OUT_LR);
            INIT_SUB_OP(OP_MULT, &OUT_R);
                DIFF_R(&OUT_RL);
                INIT_SUB_OP(OP_LOG, &OUT_RR);
                    INIT_SUB_CONST(M_E, &L(OUT_RR));
                    INIT_SUB_CONST(C(IN_L), &R(OUT_RR));
    }
    else {
        INIT_OP(OP_MULT);
            COPY_NODE(&OUT_L);
            INIT_SUB_OP(OP_ADD, &OUT_R);
                INIT_SUB_OP(OP_MULT, &OUT_RL);
                    DIFF_R(&L(OUT_RL));
                    INIT_SUB_OP(OP_LOG, &R(OUT_RL));
                        INIT_SUB_CONST(M_E, &L(R(OUT_RL)));
                        COPY_L(&R(R(OUT_RL)));
                INIT_SUB_OP(OP_DIV, &OUT_RR);
                    INIT_SUB_OP(OP_MULT, &L(OUT_RR));
                        COPY_R(&R(L(OUT_RR)));
                        DIFF_L(&L(L(OUT_RR)));
                    COPY_L(&R(OUT_RR));
    }
    return statusCode;
}

// (u log v)' = (log_u(v))' = (lnv/lnu)' = ((v'/v)lnu - (u'/u)lnv)/lnu^2
// special cases: (log_c(c))' = 0  (log_c(u))' = u'/(u lnc)
errorStatus_e DiffOpLog(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    if (T(IN_L) == CONST_T && T(IN_R) == CONST_T) {
        INIT_CONST(0);
    }
    else if (T(IN_L) == CONST_T) {
        INIT_OP(OP_DIV);
            DIFF_R(&OUT_L);
            INIT_SUB_OP(OP_MULT, &OUT_R);
                COPY_R(&OUT_RL);
                INIT_SUB_OP(OP_LOG, &OUT_RR);
                    INIT_SUB_CONST(M_E, &L(OUT_RR));
                    INIT_SUB_CONST(C(IN_L), &R(OUT_RR));
    }
    else {
        INIT_OP(OP_DIV);
            INIT_SUB_OP(OP_SUB, &OUT_L);
                INIT_SUB_OP(OP_MULT, &OUT_LL);
                    INIT_SUB_OP(OP_DIV, &L(OUT_LL));
                        DIFF_R(&L(L(OUT_LL)));
                        COPY_R(&R(L(OUT_LL)));
                    INIT_SUB_OP(OP_LOG, &R(OUT_LL));
                        INIT_SUB_CONST(M_E, &L(R(OUT_LL)));
                        COPY_L(&R(R(OUT_LL)));
                INIT_SUB_OP(OP_MULT, &OUT_LR);
                    INIT_SUB_OP(OP_DIV, &L(OUT_LR));
                        DIFF_L(&L(L(OUT_LR)));
                        COPY_L(&R(L(OUT_LR)));
                    INIT_SUB_OP(OP_LOG, &R(OUT_LR));
                        INIT_SUB_CONST(M_E, &L(R(OUT_LR)));
                        COPY_R(&R(R(OUT_LR)));
            INIT_SUB_OP(OP_DEG, &OUT_R);
                INIT_SUB_OP(OP_LOG, &OUT_RL);
                    INIT_SUB_CONST(M_E, &L(OUT_RL));
                    COPY_L(&R(OUT_RL));
                INIT_SUB_CONST(2, &OUT_RR);
    }
    return statusCode;
}

// (sin(u))' = cos(u)*u'
errorStatus_e DiffOpSin(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_MULT);

    INIT_SUB_OP(OP_COS, &OUT_L);
    COPY_R(&OUT_LR);

    DIFF_R(&OUT_R);

    return statusCode;
}

// (cos(u))' = -sin(u)*u'
errorStatus_e DiffOpCos(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_MULT);
        INIT_SUB_OP(OP_MULT, &OUT_L);
            INIT_SUB_CONST(-1, &OUT_LL);
            INIT_SUB_OP(OP_SIN, &OUT_LR);
                COPY_R(&R(OUT_LR));
        DIFF_R(&OUT_R);

    return statusCode;
}

// (tan(u))' = (u')/cos(u)^2
errorStatus_e DiffOpTan(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_COS, &OUT_RL);
                COPY_R(&R(OUT_RL));
            INIT_SUB_CONST(2, &OUT_RR);


    return statusCode;
}

// (cot(u))' = -1*(u')/sin(u)^2
errorStatus_e DiffOpCot(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        INIT_SUB_OP(OP_MULT, &OUT_L);
            INIT_SUB_CONST(-1, &OUT_LL);
            DIFF_R(&OUT_LR);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_SIN, &OUT_RL);
                COPY_R(&R(OUT_RL));
            INIT_SUB_CONST(2, &OUT_RR);


    return statusCode;
}

// (arcsin(u))' = u'/(sqrt(1 - u^2))
errorStatus_e DiffOpAsin(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_SUB, &OUT_RL);
                INIT_SUB_CONST(1, &L(OUT_RL));
                INIT_SUB_OP(OP_DEG, &R(OUT_RL));
                    COPY_R(&L(R(OUT_RL)));
                    INIT_SUB_CONST(2, &R(R(OUT_RL)));
            INIT_SUB_CONST(0.5, &OUT_RR);

    return statusCode;
}

// (arccos(u))' = -u'/(sqrt(1 - u^2))
errorStatus_e DiffOpAcos(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        INIT_SUB_OP(OP_MULT, &OUT_L);
            INIT_SUB_CONST(-1, &OUT_LL);
            DIFF_R(&OUT_LR);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_SUB, &OUT_RL);
                INIT_SUB_CONST(1, &L(OUT_RL));
                INIT_SUB_OP(OP_DEG, &R(OUT_RL));
                    COPY_R(&L(R(OUT_RL)));
                    INIT_SUB_CONST(2, &R(R(OUT_RL)));
            INIT_SUB_CONST(0.5, &OUT_RR);

    return statusCode;
}

// (arctan(u))' = u'/(1 + u^2))
errorStatus_e DiffOpAtan(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_ADD, &OUT_R);
            INIT_SUB_CONST(1, &OUT_RL);
            INIT_SUB_OP(OP_DEG, &OUT_RR);
                COPY_R(&L(OUT_RR));
                INIT_SUB_CONST(2, &R(OUT_RR));

    return statusCode;
}

// (arccot(u))' = -u'/(1 + u^2))
errorStatus_e DiffOpAcot(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        INIT_SUB_OP(OP_MULT, &OUT_L);
            INIT_SUB_CONST(-1, &OUT_LL);
            DIFF_R(&OUT_LR);
        INIT_SUB_OP(OP_ADD, &OUT_R);
            INIT_SUB_CONST(1, &OUT_RL);
            INIT_SUB_OP(OP_DEG, &OUT_RR);
                COPY_R(&L(OUT_RR));
                INIT_SUB_CONST(2, &R(OUT_RR));

    return statusCode;
}

// (sh(u))' = u'*ch(u)
errorStatus_e DiffOpSh(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_MULT);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_CH, &OUT_R);
            COPY_R(&OUT_RR);

    return statusCode;
}

// (ch(u))' = u'*sh(u)
errorStatus_e DiffOpCh(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_MULT);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_SH, &OUT_R);
            COPY_R(&OUT_RR);

    return statusCode;
}

// (th(u))' = (u')/ch(u)^2
errorStatus_e DiffOpTh(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_CH, &OUT_RL);
                COPY_R(&R(OUT_RL));
            INIT_SUB_CONST(2, &OUT_RR);


    return statusCode;
}

// (cth(u))' = -1*(u')/sh(u)^2
errorStatus_e DiffOpCth(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        INIT_SUB_OP(OP_MULT, &OUT_L);
            INIT_SUB_CONST(-1, &OUT_LL);
            DIFF_R(&OUT_LR);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_SH, &OUT_RL);
                COPY_R(&R(OUT_RL));
            INIT_SUB_CONST(2, &OUT_RR);


    return statusCode;
}

// (arcsh(u))' = u'/(sqrt(1 + u^2))
errorStatus_e DiffOpAsh(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_ADD, &OUT_RL);
                INIT_SUB_CONST(1, &L(OUT_RL));
                INIT_SUB_OP(OP_DEG, &R(OUT_RL));
                    COPY_R(&L(R(OUT_RL)));
                    INIT_SUB_CONST(2, &R(R(OUT_RL)));
            INIT_SUB_CONST(0.5, &OUT_RR);

    return statusCode;
}

// (arcch(u))' = u'/(sqrt(-1 + u^2))
errorStatus_e DiffOpAch(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_DEG, &OUT_R);
            INIT_SUB_OP(OP_ADD, &OUT_RL);
                INIT_SUB_CONST(-1, &L(OUT_RL));
                INIT_SUB_OP(OP_DEG, &R(OUT_RL));
                    COPY_R(&L(R(OUT_RL)));
                    INIT_SUB_CONST(2, &R(R(OUT_RL)));
            INIT_SUB_CONST(0.5, &OUT_RR);

    return statusCode;
}

// (arcth(u))' = (arccthu))' = u'/(1 - u^2)) 
errorStatus_e DiffOpAthOrActh(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff)
{
    assert(inNode);
    assert(outNodePtr);
    errorStatus_e statusCode = OK;

    INIT_OP(OP_DIV);
        DIFF_R(&OUT_L);
        INIT_SUB_OP(OP_SUB, &OUT_R);
            INIT_SUB_CONST(1, &OUT_RL);
            INIT_SUB_OP(OP_DEG, &OUT_RR);
                COPY_R(&L(OUT_RR));
                INIT_SUB_CONST(2, &R(OUT_RR));

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

errorStatus_e ConstFold(binTreeNode_t *inNode)
{
    if (!inNode) return OK;
    if (IN_TYPE == CONST_T || IN_TYPE == VAR_T) return OK;

    if (ConstFold(IN_L) != OK) return WRIN;
    if (ConstFold(IN_R) != OK) return WRIN;

    if (IN_L && T(IN_L) == CONST_T && T(IN_R) == CONST_T) {
        switch (IN_OP)
        {
        case OP_ADD:
            IN_CONST = C(IN_L) + C(IN_R);
            break;
        case OP_SUB:
            IN_CONST = C(IN_L) - C(IN_R);
            break;
        case OP_MULT:
            IN_CONST = C(IN_L) * C(IN_R);
            break;
        case OP_DIV:
            if (IsZero(C(IN_R))) return $ERR(WRIN, "DIVISION BY ZERO");
            IN_CONST = C(IN_L) / C(IN_R);
            break;
        case OP_LOG:
            if (C(IN_R) <= 0 || C(IN_L) <= 0 || IsZero(C(IN_L) - 1)) return $ERR(WRIN, "WRONG LOG ARGUMENTS");
            IN_CONST = log(C(IN_R)) / log(C(IN_L));
            break;
        case OP_DEG:
            IN_CONST = pow(C(IN_L), C(IN_R));
            if (isnan(IN_CONST) || isinf(IN_CONST)) return $ERR(WRIN, "WRONG POW ARGUMENTS");
            break;
        default:
            return $ERR(WRIN, "UNSUPPORTED OPERATION");
        }
        TreeNodeDelete(&IN_L);
        TreeNodeDelete(&IN_R);
        IN_TYPE = CONST_T;
    }

    return OK;
}

errorStatus_e AlgebraSimpl(binTreeNode_t **node)
{
    assert(node);
    binTreeNode_t *inNode = *node;
    errorStatus_e statusCode = OK;
    binTreeNodeVal_t val = {};

    if (!inNode) return OK;
    if (IN_TYPE == CONST_T || IN_TYPE == VAR_T) return OK;

    RUN_COMMAND(AlgebraSimpl(&IN_L), "PROBLEM WHILE CHECKING LEFT SUBTREE"); 
    RUN_COMMAND(AlgebraSimpl(&IN_R), "PROBLEM WHILE CHECKING RIGHT SUBTREE");

    if (!IN_R || !IN_L) return OK;

    switch (IN_OP)
    {
        case OP_ADD:
            if      (T(IN_L) == CONST_T && IsZero(C(IN_L))) RelocateNode(node, IN_L, IN_R);
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R))) RelocateNode(node, IN_R, IN_L);
            return OK;
        case OP_SUB:
            if      (T(IN_R) == CONST_T && IsZero(C(IN_R)))                          RelocateNode(node, IN_R, IN_L);
            else if (T(IN_L) == VAR_T && T(IN_R) == VAR_T && VAR(IN_L) == VAR(IN_R)) {REPLACE_WITH(0);}
            return OK;
        case  OP_MULT:
            if      (T(IN_L) == CONST_T && IsZero(C(IN_L)))     {REPLACE_WITH(0);}
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R)))     {REPLACE_WITH(0);}
            else if (T(IN_L) == CONST_T && IsZero(C(IN_L) - 1)) RelocateNode(node, IN_L, IN_R);
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R) - 1)) RelocateNode(node, IN_R, IN_L);
            return OK;
        case OP_DIV:
            if      (T(IN_L) == CONST_T && IsZero(C(IN_L)))                          {REPLACE_WITH(0);}
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R) - 1))                      RelocateNode(node, IN_R, IN_L);
            else if (T(IN_L) == VAR_T && T(IN_R) == VAR_T && VAR(IN_L) == VAR(IN_R)) {REPLACE_WITH(1);}
            return OK;
        case OP_DEG:
            if      (T(IN_L) == CONST_T && IsZero(C(IN_L) - 1)) {REPLACE_WITH(1);}
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R)))     {REPLACE_WITH(1);}
            else if (T(IN_L) == CONST_T && IsZero(C(IN_L)))     {REPLACE_WITH(0);}
            else if (T(IN_R) == CONST_T && IsZero(C(IN_R) - 1)) RelocateNode(node, IN_R, IN_L);
            return OK;
        case OP_LOG:
            if (T(IN_R) == CONST_T && IsZero(C(IN_R) - 1)) {REPLACE_WITH(0);}
            return OK;
        default:
            return OK;
    }
}

errorStatus_e RelocateNode(binTreeNode_t **node, binTreeNode_t *delNode, binTreeNode_t *reconNode)
{
    assert(node);
    assert(delNode);
    assert(reconNode);

    TreeNodeDelete(&delNode);
    free(*node);
    *node = reconNode;

    return OK;
}