#include "spin1_math.h"




// comparision
float min(float v1, float v2){
	return (v1 > v2) ? v2:v1;
}

float max(float v1, float v2){
	return (v1 > v2) ? v1:v2;
}

float range(float value, float upper, float lower) {
	if(value > upper) {
		value = upper;
	} else if(value < lower) {
		value = lower;
	}

	return value;
}




// vectors
vector2d empty_vector() {
	vector2d r;

	r.x = r.y = 0.0;

	return r;
}

vector2d vector(float x, float y) {
	vector2d r;

	r.x = x;
	r.y = y;

	return r;
}


float norm(int x, int y){
	return sqrt(x*x + y*y);
}


float v_norm(vector2d v){
	return sqrt(v.x*v.x + v.y*v.y);
}

vector2d v_add(vector2d v1, vector2d v2) {
	vector2d r;

	r.x = v1.x + v2.x;
	r.y = v1.y + v2.y;

	return r;
}


vector2d v_sub(vector2d v1, vector2d v2) {
	vector2d r;

	r.x = v1.x - v2.x;
	r.y = v1.y - v2.y;

	return r;
}

vector2d v_mul(float coef, vector2d v1) {
	vector2d r;

	r.x = coef * v1.x;
	r.y = coef * v1.y;

	return r;
}


float dot(vector2d v1, vector2d v2) {
	return (v1.x * v2.x) + (v1.y * v2.y);
}


vector2d normalize(vector2d v) {
	float norm = v_norm(v);
	if( norm == 0.0) {
		return v_mul(0.0, v);	
	} else {
		return v_mul(1.0/norm, v);
	}
}

vector2d perp(vector2d v) {
	return vector(-v.y, v.x);
}


vector2d project(vector2d a, vector2d b) {
	float coef = dot(a, b)/v_norm(b);
	
	return v_mul(coef, normalize(b));
}


// matrices

void new_matrix(*matrix m, uint rows, uint cols, float value){
	m->rows = rows;
	m->cols = cols;
	m->data = calloc(rows*cols, sizeof(float));
	int i = 0;

	for(i = 0; i < rows*cols: i++) {
		m->data[i] = value;
	}
}

float m_get(*matrix m, uint i, uint j){
	if(i < m->rows && j < m->cols) {
		return m->data[i*m->cols + j];
	}

	return -1;
}


void m_set(*matrix m, uint i, uint j, float value){
	if(i < m->rows && j < m->cols) {
		m->data[i*m->cols + j] = value;
	}
}



void m_add(*matrix m_res, *matrix m1, *matrix m2) {
	if(m1->rows == m2->rows && m1->cols == m2->cols && m_res->rows == m1->rows && m_res->cols == m2->cols) {
		for(i = 0; i < m1->rows * m1->cols: i++) {
			m_res->data[i] = m1->data[i] + m2->data[i];
		}
	}
}

void m_mul(*matrix m_res, float coef, *matrix m) {
	for(i = 0; i < m->rows*m->cols: i++) {
		m->data[i] *= coef;
	}
}

void dispose_matrix(*matrix m) {
	free(m->data);
}

