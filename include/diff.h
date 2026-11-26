#ifndef DIFF_H
#define DIFF_H

#include "basic.h"
#include "binTree.h"

// -------------------  DSL --------------------------------------

#define IN_TYPE   ((inNode -> val).nodeType         )
#define IN_CONST  ((inNode -> val).nodeVal.constVal )
#define IN_VAR    ((inNode -> val).nodeVal.varVal   )
#define IN_OP     ((inNode -> val).nodeVal.opVal    )
#define IN_L      (inNode -> left )
#define IN_R      (inNode -> right)

#define OUT_TYPE  (((*outNodePtr) -> val).nodeType         )
#define OUT_CONST (((*outNodePtr) -> val).nodeVal.constVal )
#define OUT_VAR   (((*outNodePtr) -> val).nodeVal.varVal   )
#define OUT_OP    (((*outNodePtr) -> val).nodeVal.opVal    )
#define OUT_L     ((*outNodePtr) -> left )
#define OUT_R     ((*outNodePtr) -> right)
#define OUT_LL    (((*outNodePtr) -> left ) -> left )
#define OUT_LR    (((*outNodePtr) -> left ) -> right)
#define OUT_RL    (((*outNodePtr) -> right) -> left )
#define OUT_RR    (((*outNodePtr) -> right) -> right)

#define L(ptr) ((ptr) -> left)
#define R(ptr) ((ptr) -> right)

#define INIT_CONST(cVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = CONST_T;      \
                    val.nodeVal.constVal = cVal;         \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING CONST NODE");

#define INIT_SUB_CONST(cVal, conPtr)                     \
                    val.nodeType         = CONST_T;      \
                    val.nodeVal.constVal = cVal;         \
                    RUN_COMMAND(TreeNodeCreate(val, conPtr), "PROBLEM WHILE CREATING CONST NODE");

#define INIT_VAR(vVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = VAR_T;      \
                    val.nodeVal.varVal   = vVal;       \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING VAR NODE");

#define INIT_OP(oVal)  binTreeNodeVal_t val = {};     \
                    val.nodeType         = OP_T;      \
                    val.nodeVal.opVal    = oVal;      \
                    RUN_COMMAND(TreeNodeCreate(val, outNodePtr), "PROBLEM WHILE CREATING OP NODE");

#define INIT_SUB_OP(oVal, conPtr)                                 \
                    val.nodeType         = OP_T;                  \
                    val.nodeVal.opVal    = oVal;                  \
                    RUN_COMMAND(TreeNodeCreate(val, conPtr), "PROBLEM WHILE CREATING OP NODE");


#define DIFF_L(conPtr)    RUN_COMMAND(Diff(IN_L, conPtr, varToDiff), "PROBLEM WHILE DIFF FIND");
#define DIFF_R(conPtr)    RUN_COMMAND(Diff(IN_R, conPtr, varToDiff), "PROBLEM WHILE DIFF FIND");

#define COPY_L(conPtr)    RUN_COMMAND(CopyTreeNode(IN_L, conPtr), "PROBLEM WHILE COPYING LEFT SON TO NEW TREE");
#define COPY_R(conPtr)    RUN_COMMAND(CopyTreeNode(IN_R, conPtr), "PROBLEM WHILE COPYING LEFT SON TO NEW TREE");
// -------------------  !DSL --------------------------------------

errorStatus_e FindDerivative(const char *inFileName);
errorStatus_e Diff(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);

errorStatus_e CopyTreeNode(binTreeNode_t *inNode, binTreeNode_t **outNodePtr);
errorStatus_e GetVarToDiff(bool *varsExistList, vars_e *resVar);

errorStatus_e DiffConst(binTreeNode_t **outNodePtr);
errorStatus_e DiffVar(vars_e var, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOp(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);

errorStatus_e DiffOpSum  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpMult (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpSub  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpDiv  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpDeg  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpLog  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpSin  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpCos  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpTan  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpCot  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAsin (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAcos (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAtan (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAcot (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpSh   (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpCh   (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpTh   (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpCth  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAsh  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAch  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpAth  (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);
errorStatus_e DiffOpActh (binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff);

errorStatus_e (*const diffFuncList[])(binTreeNode_t *inNode, binTreeNode_t **outNodePtr, vars_e varToDiff) = {
    DiffOpSum,
    DiffOpMult,
    DiffOpSub,
    DiffOpDiv,
    DiffOpDeg,
    DiffOpLog,
    DiffOpSin,
    DiffOpCos,
    DiffOpTan,
    DiffOpCot,
    DiffOpAsin,
    DiffOpAcos,
    DiffOpAtan,
    DiffOpAcot,
    DiffOpSh,
    DiffOpCh,
    DiffOpTh,
    DiffOpCth,
    DiffOpAsh,
    DiffOpAch,
    DiffOpAth,
    DiffOpActh,
};

#endif
