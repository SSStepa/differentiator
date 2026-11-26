#ifndef DIFF_H
#define DIFF_H

#include "basic.h"
#include "binTree.h"

// -------------------  DSL --------------------------------------

#define IN_TYPE   (inNode -> val).nodeType
#define IN_CONST  (inNode -> val).nodeVal.constVal
#define IN_VAR    (inNode -> val).nodeVal.varVal
#define IN_OP     (inNode -> val).nodeVal.opVal

#define OUT_TYPE  ((*outNodePtr) -> val).nodeType
#define OUT_CONST ((*outNodePtr) -> val).nodeVal.constVal
#define OUT_VAR   ((*outNodePtr) -> val).nodeVal.varVal
#define OUT_OP    ((*outNodePtr) -> val).nodeVal.opVal

#define INIT_CONST(cVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = CONST_T;      \
                    val.nodeVal.constVal = cVal;         \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING CONST NODE");

#define INIT_VAR(vVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = VAR_T;      \
                    val.nodeVal.varVal   = vVal;       \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING VAR NODE");

#define INIT_OP(oVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = OP_T;      \
                    val.nodeVal.opVal    = oVal;      \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING OP NODE");

// -------------------  !DSL --------------------------------------

errorStatus_e FindDerivative(const char *inFileName);
errorStatus_e Diff(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e GetVarToDiff(bool *varsExistList, vars_e *resVar);

errorStatus_e DiffConst(binTreeNode_t **outNodePtr);
errorStatus_e DiffVar(vars_e var, binTreeNode_t **outNodePtr, vars_e varToDiff);
#endif
