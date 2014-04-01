#ifndef MATH_H
#define MATH_H

#include "fixed_float.h"

#define INF_SERIES_PRECISION 10
#define PI_ 3.14159265359
#define E_  2.71828182845
#define SQRT_STEPS 6

// basic operations
long l_abs_(long n);
float f_abs_(float n);
long l_min_(long n1, long n2);
float f_min_(float n1, float n2);
long l_max_(long n1, long n2);
float f_max_(float n1, float n2);

// powers and factorials
long fact_(unsigned int n);
float pow_(float base, int exp);
float exp_(float power);
float sqrt_(unsigned int n);

// trigonometric functions
float sin_(float theta);
float cos_(float theta);
float tan_(float theta);

float csc_(float theta);
float sec_(float theta);
float cot_(float theta);

float arcsin_(float z);
float arccos_(float z);
float arctan_(float z);

// combinatorics
long comb_(unsigned int n, unsigned int k);
long perm_(unsigned int n, unsigned int k);

// vectors
float norm_(int x, int y);

#endif



