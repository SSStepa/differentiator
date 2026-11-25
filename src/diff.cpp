#include "../include/diff.h"



errorStatus_e Diff(const char *inFileName)
{
    assert(inFileName);
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

    RUN_COMMAND(ReadDataAndCreateTree(&inTree, &str, inFile, ))


    binTree_t outTree = {};

    RUN_COMMAND(FindDerivative(inTree.data, &outTree.data), "PROBLEM WHILE TRYING TO DERIVATE");

    $INFO("ending diff function");
    return statusCode;
}

errorStatus_e FindDerivative(binTreeNode_t *inNode, binTreeNode_t **outNodePtr)
{
    
}