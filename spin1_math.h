#ifndef MATH_H
#define MATH_H

#include "math.h"

#define PI_ 3.141527


// comparision
float min(float v1, float v2);
float max(float v1, float v2);


// vectors
struct vector2d {
	float x;
	float y;
};

typedef struct vector2d vector2d;

vector2d empty_vector();
vector2d vector(float x, float y);

float norm(int x, int y);
float v_norm(vector2d v);

vector2d v_add(vector2d v1, vector2d v2);
vector2d v_sub(vector2d v1, vector2d v2);
vector2d v_mul(float coef, vector2d v1);
float dot(vector2d v1, vector2d v2);

vector2d normalize(vector2d v);

#endif



