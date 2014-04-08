#include "spin1_math.h"




// comparision
float min(float v1, float v2){
	return (v1 > v2) ? v2:v1;
}

float max(float v1, float v2){
	return (v1 > v2) ? v1:v2;
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
