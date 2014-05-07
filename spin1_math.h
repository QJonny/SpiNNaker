#ifndef MATH_H
#define MATH_H

#include "math.h"

#define PI_ 3.141527


// comparision
float min(float v1, float v2);
float max(float v1, float v2);
float range(float value, float upper, float lower);

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
vector2d perp(vector2d v);

// projects a on b
vector2d project(vector2d a, vector2d b);



// matrices
struct matrix {
	float* data;
	uint rows;
	uint cols;
};

typedef struct matrix matrix;

void new_matrix(*matrix m, uint rows, uint cols, float value);
float m_get(*matrix m, uint i, uint j);
void m_set(*matrix m, uint i, uint j, float value);
void m_add(*matrix m_res, *matrix m1, *matrix m2);
void m_mul(*matrix m_res, float coef, *matrix m);
void dispose_matrix(*matrix m);

#endif



