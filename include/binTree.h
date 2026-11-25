#ifndef BIN_TREE
#define BIN_TREE

#include "basic.h"

const char *const GRAPHVIZ_TEXT_FILE = "graphvizInfo.txt";
const char *const GRAPHVIZ_OUT_BASE  = "dumpGraphviz";

const int COMMAND_MAX_LEN = 120;

#define MAX_IN_NODE 10

enum calcNodeType_e {
    CONST_T,
    VAR_T,
    OP_T
};

enum operations_e {
    OP_NULL = -1,
    OP_ADD,
    OP_MULT,
    OP_SUB, 
    OP_DIV,
    OP_DEG,
    OP_LOG,
    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_COT,
    OP_ASIN,
    OP_ACOS,
    OP_ATAN,
    OP_ACOT,
    OP_SH,
    OP_CH,
    OP_TH,
    OP_CTH,
    OP_ASH,
    OP_ACH,
    OP_ATH,
    OP_ACTH
};

struct operation_t {
    operations_e  opCode;
    const char    *name;
};

const operation_t OPERATIONS[] = {
    {OP_ADD,   "+"     },
    {OP_MULT,  "*"     },
    {OP_SUB,   "-"     },
    {OP_DIV,   "/"     },
    {OP_DEG,   "^"     },
    {OP_LOG,   "log"   },
    {OP_SIN,   "sin"   },
    {OP_COS,   "cos"   },
    {OP_TAN,   "tan"   },
    {OP_COT,   "cot"   },
    {OP_ASIN,  "asin"  },
    {OP_ACOS,  "acos"  },
    {OP_ATAN,  "atan"  },
    {OP_ACOT,  "acot"  },
    {OP_SH,    "sh"    },
    {OP_CH,    "ch"    },
    {OP_TH,    "th"    },
    {OP_CTH,   "cth"   },
    {OP_ASH,   "ash"   },
    {OP_ACH,   "ach"   },
    {OP_ATH,   "ath"   },
    {OP_ACTH,  "acth"  },

    {OP_NULL,  ""      },
};

const int VARS_AMOUNT = 3;
enum vars_e {
    VAR_X,
    VAR_Y,
    VAR_Z,
};

struct var_t {
    vars_e     varCode;
    const char name;
};

const var_t VARS[] = {
    {VAR_X, 'x'},
    {VAR_Y, 'y'},
    {VAR_Z, 'z'},
};

union nodeVal {
    double        constVal;
    operations_e  opVal;
    vars_e        varVal;
};

struct calcNode_t {
    calcNodeType_e nodeType;
    nodeVal        nodeVal; 
};

typedef calcNode_t binTreeNodeVal_t;
#define PRINTF_NODE_VAL ("%lg" (node -> val).nodeVal)

//--------------------------- DSL ------------------------------------------

#define NODE_T     (node -> val).nodeType
#define NODE_CONST (node -> val).nodeVal.constVal
#define NODE_VAR   (node -> val).nodeVal.varVal
#define NODE_OP    (node -> val).nodeVal.opVal

//--------------------------- DSL ----------------------------------------


struct binTreeNode_t {
   binTreeNodeVal_t val;
   binTreeNode_t   *left;
   binTreeNode_t   *right;
};

struct binTree_t {
    binTreeNode_t *data;

    ON_DBG(
        debuggingInfo_t debuggingData;

        binTreeNode_t *ptrToLastUsedNode;
    )
};

/**
 * creation of tree. Initializes debugging info.
 * do almost nothing in basic realization
 * 
 * @param [out] tree pointer to tree to create.
 * 
 * When debugging:
 * @param [in] variableName         name of variable in which tree is created.
 * @param [in] fileCreationName     name of file in which tree is created.
 * @param [in] functionCreationName name of function in which tree is created 
 * @param [in] lineCreationNumber   number of line in which tree is created 
 */
errorStatus_e TreeCtor(binTree_t *tree ON_DBG(, const char *variableName, const char *fileCreationName, const char *functionCreationName, const int lineCreationNumber));

/**
 * Dtor of tree. frees all nodes in dynamic memory.
 * 
 * @param [out] tree pointer to tree to deconstruct.
 */
errorStatus_e TreeDtor(binTree_t *tree);

/**
 * Dump for all Tree. Calls recursive TreeNodeDump for all tree. 
 * 
 * @param [in] tree tree to dump
 */
errorStatus_e TreeDump(const binTree_t *tree);


/**
 * Recursive function to dump one node
 * As a result dumps all tree to graphviz and out.
 * 
 * @param [in] node node to dump
 */
errorStatus_e TreeNodeDump(const binTreeNode_t *node, FILE *outGraphvizFile, FILE *logFile);

/**
 * Creates node and gives value to it.
 * Sets left and right children to 0. connects created node to passed pointer
 * 
 * @param [in] val value which is given to new node 
 * @param [out] createdNodePtr pointer in which new node pointer to is saved(so it can be field in parent node) 
 */
errorStatus_e TreeNodeCreate(binTreeNodeVal_t val, binTreeNode_t **createdNodePtr);

/**
 * deletes node and all it's children recursively.
 * 
 * @param [in, out] deleteNodePtr pointer to node to delete
 */
errorStatus_e TreeNodeDelete(binTreeNode_t **deleteNodePtr);

/**
 * prints tree recursively in preorder form
 * 
 * @param [in] node node to print
 */
errorStatus_e TreePrintPreorder(const binTreeNode_t *node);

/**
 * prints tree recursively in inorder form
 * 
 * @param [in] node node to print
 */
errorStatus_e TreePrintInorder(const binTreeNode_t *node);

/**
 * prints tree recursively in postorder form
 * 
 * @param [in] node node to print
 */
errorStatus_e TreePrintPostorder(const binTreeNode_t *node);

/**
 * macro to call TreeCtor with debugging info args auto adding.
 */
#define TREE_CTOR(tree) TreeCtor(tree ON_DBG(, TO_STR(tree), __FILE__, __FUNCTION__, __LINE__));

/**
 * macro to call TreeNodeCreatee with debuging info add
 */
#define TREE_NODE_CREATE(val, createdNodePtr, tree)     \
    TreeNodeCreate(val, createdNodePtr);                 \
    ON_DBG(                                             \
        (tree).ptrToLastUsedNode = *(createdNodePtr);     \
        TreeDump(&(tree));                              \
    )                       

/**
 * macro to call TreeDeleteCreate with debuging info add
 */
#define TREE_NODE_DELETE(deleteNodePtr, tree)           \
    TreeNodeDelete(deleteNodePtr);                      \
    ON_DBG(                                             \
        tree.ptrToLastUsedNode = *(deleteNodePtr);      \
        TreeDump(&(tree));                              \
    )

errorStatus_e ReadDataAndCreateTree(binTree_t *tree, string_t *inString, FILE *inFile, double *varsList);
errorStatus_e AddNodeToTree(binTreeNode_t **ptrToConnect, char **ptrToRead, double *varsList);
errorStatus_e GetNodeVal(binTreeNodeVal_t *outVal, char **ptrToRead, double *varsList);
errorStatus_e AddVarValue(double *varsList, size_t ind);
bool CheckSymbolAndMove(char **ptrLineToCheck, char Symbol);

#endif