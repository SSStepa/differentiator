#ifndef CALC_H
#define CALC_H

#include "basic.h"
#include "binTree.h"


errorStatus_e Calc(const char *inFileName);
errorStatus_e DoCalcOperations(binTreeNode_t *node, double *outVal, double* varList);
errorStatus_e AddVarValue(double *varsList, bool *varsExistList);

#endif