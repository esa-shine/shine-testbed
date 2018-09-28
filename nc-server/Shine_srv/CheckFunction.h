#ifndef CHECKFUNCTION_H
#define CHECKFUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void check_memory_allocation_1D(int *pt, string msg);
void check_memory_allocation_2D(int **pt, string msg);
void check_memory_allocation_3D(int ***pt, string msg);
void check_memory_double_allocation_1D(double *pt, string msg);
void check_memory_double_allocation_2D(double **pt, string msg);

#endif // CHECKFUNCTION_H
