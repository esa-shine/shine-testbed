#ifndef DECODINGINPUT_H
#define DECODINGINPUT_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CheckFunction.h"
#include "DataDefinition.h"

using namespace std;

void _allocArrays(int, int*, int);
data_matrix fromArrayToMatrix(int, int*, vector<int>);
void computeQMatrix(int, int*);
void computeIndMatrix(vector<int>);

#endif
