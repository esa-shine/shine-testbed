#ifndef GRASP_H_INCLUDED
#define GRASP_H_INCLUDED

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "randomHandler.h"
#include "DataDefinition.h"
#include "heapSort.h"

using namespace std;

/* GRASP Functions */
void graspProcess();
void runGrasp();
int *constructGreedyRandomizedSolution(int *fo, float alpha);
RCL_LIST *makeRCL(float alpha, int *n_rcl, int *sol);
int *localSearch(int *sol, int *fo);

void checkConstraints(int *sol);

int getColor(NODE *node, int *fo, int *sol);

void createGraph(cf_data outputForColoring);

int *graspGraphColoring(int maxIter, cf_data outputForColoring, int *n_col);

#endif // GRASP_H_INCLUDED
