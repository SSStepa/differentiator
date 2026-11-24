#ifndef CALC_H
#define CALC_H

#include "basic.h"

enum calcNodeType_e {
    CONST_T,
    VAR_T,
    OP_T
};

enum operations_e {
    OP_NULL  = 0,
    OP_ADD   = '+',
    OP_MULT  = '*',
    OP_SUB   = '-', 
    OP_DIV   = '/',
    OP_SIN   = 's',

};

struct operation_t {
    operations_e  opCode;
    const char    *name;
};

const operation_t OPERATIONS[] = {
    {OP_ADD,   "+"  },
    {OP_MULT,  "*"  },
    {OP_SUB,   "-"  },
    {OP_DIV,   "/"  },
    {OP_SIN,   "sin"},

    {OP_NULL,  ""   },
};

union nodeVal {
    double        constVal;
    operations_e  opVal;
    char          varVal;
};

struct calcNode_t {
    calcNodeType_e nodeType;
    nodeVal        nodeVal; 
};
typedef calcNode_t binTreeNodeVal_t;
#define PRINTF_NODE_VAL ("%lg" (node -> val).nodeVal)

#include "../src/binTree.cpp"




#endif