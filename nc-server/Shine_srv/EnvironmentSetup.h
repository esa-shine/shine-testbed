#ifndef ENVIRONMENTSETUP_H
#define ENVIRONMENTSETUP_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "CheckFunction.h"
#include "randomHandler.h"
#include "DataDefinition.h"

/* Include GSL header files. */
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

using namespace std;

void randomIndMatrix(float, int **, vector<int> &, int, vector<int>);
void randomQVector(double **);
//void check_Ind_Matrix();

void _allocVariables();
void _deallocVariables();

vector <int> setEnvironment(int, int, int*, double **, int **);

#endif
