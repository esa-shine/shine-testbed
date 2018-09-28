#ifndef CONFLICTGRAPH_H
#define CONFLICTGRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "randomHandler.h"
#include "CheckFunction.h"
#include "DataDefinition.h"

/* Include GSL header files. */
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

void _dealloc();

void computeNumberOfNodes();

void makeNodes();
void makeEdges();

cf_data conflictGraphGenerator(data_matrix data);

#endif // CONFLICTGRAPH_H
