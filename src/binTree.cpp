#include "../include/binTree.h"

#include <stdio.h>

errorStatus_e TreeCtor(binTree_t *tree
     ON_DBG(, const char *variableName, const char *fileCreationName, const char *functionCreationName, const int lineCreationNumber)
)
{
    assert(tree);
    ON_DBG(
        assert(variableName);
        assert(fileCreationName);
        assert(functionCreationName);
        assert(lineCreationNumber > 0);
    )

    $INFO("Starting initing of tree");

    tree -> data = 0;

    ON_DBG(
        INIT_DEBUGGING_INFO(
            tree -> debuggingData,
            variableName,
            fileCreationName,
            functionCreationName,
            lineCreationNumber
        );
    )
    $INFO("Ending initing of tree");

    return OK;
}

errorStatus_e TreeDtor(binTree_t *tree)
{
    assert(tree);

    $INFO("Starting tree deconstruction");

    errorStatus_e statusCode = OK;

    RUN_COMMAND(TreeNodeDelete(&(tree -> data)), "PROBLEM WHILE DELETING ALL TREE");

    $INFO("Ending tree deconstruction");

    return statusCode;
}

errorStatus_e TreeDump(const binTree_t *tree)
{
    assert(tree);

    errorStatus_e statusCode = OK;
    LOG_START();

    $INFO("Starting dump of tree <%s>", (tree -> debuggingData).variableName);
    
    ON_DBG(
        LOG_PRINTF("Hi, I'm dump off tree called because of err:%d and has err message <%s>\n", (tree -> debuggingData).err, (tree -> debuggingData).errorText);
    )

    FILE *graphvizOutFile = fopen(GRAPHVIZ_TEXT_FILE, "w");
    if (graphvizOutFile == NULL) {
        return $ERR(FILEERR, "CAN'T OPEN GRAPHVIZ TEXT FILE");
    }

    fprintf(graphvizOutFile, "digraph {\n");
    fprintf(graphvizOutFile, "node [shape=Mcircle, style=filled, color=\"#800000\", fillcolor=\"#FF0000\"];\n");
    fprintf(graphvizOutFile, "edge [arrowsize=0.8, color=\"#030352\", penwidth=1];\n");

    static size_t dumpNumber = 0;
    dumpNumber++;
    TreeNodeDump(tree -> data, graphvizOutFile, logFile);

    ON_DBG(
        if (tree -> ptrToLastUsedNode)
            fprintf(graphvizOutFile, "N%p [color=\"#9e4f05\", fillcolor=\"#fa8d3f\"]\n", tree -> ptrToLastUsedNode);
    )
    fprintf(graphvizOutFile, "}\n");
    fclose(graphvizOutFile);

    char commandName[COMMAND_MAX_LEN] = {};
    sprintf(commandName, "iconv -f CP1251 -t UTF-8 %s >RECODE%s", GRAPHVIZ_TEXT_FILE, GRAPHVIZ_TEXT_FILE);
    system(commandName);
    sprintf(commandName, "\"\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" RECODE%s -Tpng -o .\\%s%zu.png\"", GRAPHVIZ_TEXT_FILE, GRAPHVIZ_OUT_BASE, dumpNumber);
    system(commandName);

    LOG_END();
    $INFO("Ending dump of stack <%s>", (tree -> debuggingData).variableName);
    return statusCode;
}

errorStatus_e TreeNodeDump(const binTreeNode_t *node, FILE *outGraphvizFile, FILE *logFile)
{
    assert(outGraphvizFile);
    assert(logFile);

    errorStatus_e statusCode = OK;

    if (node == 0)
        return OK;
    
    LOG_PRINTF("(");
    RUN_COMMAND(TreeNodeDump(node -> left, outGraphvizFile, logFile), "PROBLEM WHILE PRINTING LEFT CHILD");

    switch((node -> val).nodeType) {
        case CONST_T:
            fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{My addr: %p | const val: %lg | {left: %p | right: %p}}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n",
                        node, node, (node -> val).nodeVal.constVal, node -> left, node -> right);
            LOG_PRINTF("const: %lg", (node -> val).nodeVal.constVal);
            break;
        case VAR_T:
            fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{My addr: %p | var: %d | {left: %p | right: %p}}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n",
                        node, node, (node -> val).nodeVal.varVal, node -> left, node -> right);
            LOG_PRINTF("var: %c", (node -> val).nodeVal.varVal);
            break;
        case OP_T:
            fprintf(outGraphvizFile, "N%p [shape=Mrecord, label=\"{My addr: %p | OP: %c | {left: %p | right: %p}}\", fillcolor=\"#C0C0FF\" color=\"#000080\"]\n",
                        node, node, (node -> val).nodeVal.opVal, node -> left, node -> right);
            LOG_PRINTF("op: %c", (node -> val).nodeVal.opVal);
            break;
        default:
            return $ERR(WRIN, "UNSUPPORTED NODE TYPE");
    }

    if (node -> left)
        fprintf(outGraphvizFile, "N%p -> N%p\n", node, node -> left);
    if (node -> right)
        fprintf(outGraphvizFile, "N%p -> N%p\n", node, node -> right);

    RUN_COMMAND(TreeNodeDump(node -> right, outGraphvizFile, logFile), "PROBLEM WHILE PRINTING RIGHT CHILD");
    LOG_PRINTF(")");

    return OK;
}

errorStatus_e TreeNodeCreate(binTreeNodeVal_t val, binTreeNode_t **createdNodePtr)
{
    assert(createdNodePtr);
    $INFO("Starting creating new node which will be connected to <%p>", createdNodePtr);

    binTreeNode_t* newNode = (binTreeNode_t *) calloc(1, sizeof(binTreeNode_t));
    if (newNode == NULL) {
        return $ERR(NOMEM, "NOT ENOUGH MEMORY TO CREATE NEW NODE");
    }

    newNode -> val = val;
    *createdNodePtr = newNode;
    
    $INFO("Ending creating new node which will be connected to <%p>", createdNodePtr);
    return OK;
}

errorStatus_e TreeNodeDelete(binTreeNode_t **deleteNodePtr)
{
    assert(deleteNodePtr);
    $INFO("Staring deleting node which will is connected to <%p>", deleteNodePtr);

    if (*deleteNodePtr == NULL)
        return OK;

    TreeNodeDelete(&((*deleteNodePtr) -> left));
    TreeNodeDelete(&((*deleteNodePtr) -> right));

    free(*deleteNodePtr);
    *deleteNodePtr = 0;

    $INFO("Ending deleting node which will is connected to <%p>", deleteNodePtr);
    return OK;
}

errorStatus_e ReadDataAndCreateTree(binTree_t *tree, string_t *inString, FILE *inFile, double *varsList)
{
    assert(tree);
    assert(inString);
    assert(inFile);

    $INFO("starting getting info and creating tree");
    errorStatus_e statusCode = OK;

    ssize_t len =  my_getline(&(inString -> str), &(inString -> len), inFile);

    if (len <= 0)
        return $ERR(WRIN, "PROBLEM WHILE TAKING DATA FROM USER");
    inString -> len = (size_t) len;

    TREE_CTOR(tree);

    RUN_COMMAND(AddNodeToTree(&(tree -> data), &(inString -> str), varsList), "ERROR WHILE RECURSIVELY TRANSLATING STRING TO TREE");
    inString -> str -= inString -> len; // put ptr back to first char

    TreeDump(tree);

    $INFO("ending getting info and creating tree");
    return statusCode;
}

errorStatus_e AddNodeToTree(binTreeNode_t **ptrToConnect, char **ptrToRead, double *varsList)
{
    assert(ptrToRead);
    assert(*ptrToRead);
    assert(ptrToConnect);

    $INFO("starting adding node to tree from position %.10s", *ptrToRead);
    errorStatus_e statusCode = OK;

    SkipSpaces(ptrToRead);

    if (!CheckSymbolAndMove(ptrToRead, '('))
        return WRIN;

    binTreeNode_t *dummyNode = 0;

    SkipSpaces(ptrToRead);
    AddNodeToTree(&dummyNode, ptrToRead, varsList);
    SkipSpaces(ptrToRead);

    binTreeNodeVal_t val = {};
    RUN_COMMAND(GetNodeVal(&val, ptrToRead, varsList), "PROBLEM WHILE GETTING NODE VAL INFO");
    RUN_COMMAND(TreeNodeCreate(val, ptrToConnect), "ERROR WHILE CREATING NODE");
    SkipSpaces(ptrToRead);

    if (dummyNode)
        (**ptrToConnect).left = dummyNode;

    if (val.nodeType == OP_T)
        RUN_COMMAND(AddNodeToTree(&((*ptrToConnect) -> right), ptrToRead, varsList), "PROBLEM WHILE ADDING RIGHT SON(ALL FUNCTIONS HAVE IT)");

    if (!CheckSymbolAndMove(ptrToRead, ')'))
        return $ERR(WRIN, "NO CLOSIND ) FOR NODE");

    return OK;
}

bool CheckSymbolAndMove(char **ptrLineToCheck, char Symbol)
{
    assert(ptrLineToCheck);
    assert(*ptrLineToCheck);

    if (**ptrLineToCheck != Symbol) 
        return false;
    
    (*ptrLineToCheck)++;

    return true;
}

errorStatus_e GetNodeVal(binTreeNodeVal_t *outVal, char **ptrToRead, double *varsList)
{
    assert(outVal);
    assert(ptrToRead);
    assert(*ptrToRead);

    int readLen = 0;

    if (sscanf(*ptrToRead, "%lg%n", &((outVal -> nodeVal).constVal), &readLen) == 1) {
        outVal -> nodeType = CONST_T;
    
    } else {
        char *buff = (char *) calloc(MAX_IN_NODE, sizeof(char));
        if (sscanf(*ptrToRead, "%" NUM_TO_STR(MAX_IN_NODE) "[^ \t\n()]%n", buff, &readLen) != 1) {
            return $ERR(WRIN, "INCORRECT USER INPUT");
        }
       
        size_t ind = 0;
        for (; OPERATIONS[ind].opCode != OP_NULL; ind++) {
            if (strncmp(buff, OPERATIONS[ind].name, MAX_IN_NODE) == 0) {
                outVal  -> nodeType       = OP_T;
                (outVal -> nodeVal).opVal = OPERATIONS[ind].opCode;
                break;
            }
        }
        if (OPERATIONS[ind].opCode == OP_NULL) {
            if (readLen == 1) {
                outVal  -> nodeType        = VAR_T;
                for (ind = 0; ind < VARS_AMOUNT; ind++) {
                    if (*buff == VARS[ind].name) {
                        (outVal -> nodeVal).varVal = VARS[ind].varCode;
                        AddVarValue(varsList, ind);
                        break;
                    }
                }
                if (ind == VARS_AMOUNT) return $ERR(WRIN, "UNSUPPORTED VAR USED %c", *buff);
            }
            else return $ERR(WRIN, "INCORRECT NODE FORMAT");
        }
    }
    *ptrToRead += readLen;

    return OK;
}

errorStatus_e AddVarValue(double *varsList, size_t ind)
{
    assert(varsList);

    if (isnan(varsList[ind])) {
        printf("What val you want %c to have: ", VARS[ind].name);
        if (scanf("%lg", varsList + ind) != 1)
            return $ERR(WRIN, "INCORRECT USER INPUT WHILR GETTING VAL OF %c", VARS[ind].name);
    }

    return OK;
}