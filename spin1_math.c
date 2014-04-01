#include "spin1_math.h"


// basic operations
long l_abs_(long n){
	return n < 0 ? -n : n;
}

float f_abs_(float n){
	return n < 0 ? -n : n;
}

long l_min_(long n1, long n2){
	return n1 < n2 ? n1 : n2;
}

float f_min_(float n1, float n2){
	return n1 < n2 ? n1 : n2;
}

long l_max_(long n1, long n2){
	return n1 < n2 ? n2 : n1;
}

float f_max_(float n1, float n2){
	return n1 < n2 ? n2 : n1;
}



// powers and factorials
long fact_array[21] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800,
		       3991680, 47900160, 622702080, 8717829120, 130767436800,
		       2092278988800, 35568742809600, 640237370572800, 12164510040883200,
                       243290200817664000};

long fact_(unsigned int n){
	long result = 1;
	int i = 1;

	if(n < 21) {
		return fact_array[n];
	}
	else {
		for(i=1;i <= n; i++){
			result *= i;
		}
	}

	return result;
}

float pow_(float base, int exp){
	float result = 1;
	int i = 0;

	if(exp < 0) {
		base = 1.0 / base;
	}
	else if(exp == 0){
		return 1.0;
	}

	for(i = 0; i < l_abs_(exp); i++) {
		result *= base;
	}

	return result;
}

float exp_(float power){
	int n = 0;
	float result = 0.0;

	for(n = 0; n < INF_SERIES_PRECISION; n++){
		result += pow_(power, n) / fact_(n);
	}

	return result;
}

float sqrt_(unsigned int n){
	// initial estimate
	int exp = 0;
	int i = 0;
	unsigned int tmp = n;
	float estimate = 0;
	float error = 0.0;

	if(n == 0) {
		return 0;
	}

        while(tmp > 1)
        {
		tmp = tmp >> 1; 
                exp++;
        }

        estimate = (float)(1 << (exp/2)); // 2^(exp/2)

	// babylonian algorithm
	for(i = 0; i < SQRT_STEPS; i++){
		error = (n - estimate*estimate)/(2*estimate);
		estimate = estimate + error;
	}

	return estimate;
}




// trigonometric functions
float sin_(float theta){
	int n = 0;
	float result = 0;

	for(n = 0; n < INF_SERIES_PRECISION; n++){
		result += (pow_(-1.0, n)*pow_(theta, 2*n + 1)) / fact_(2*n + 1);
	}

	return result;
}

float cos_(float theta){
	int n = 0;
	float result = 0;

	for(n = 0; n < INF_SERIES_PRECISION; n++){
		result += (pow_(-1.0, n)*pow_(theta, 2*n)) / fact_(2*n);
	}

	return result;
}

float tan_(float theta){
	return sin_(theta) / cos_(theta);
}


float csc_(float theta){
	return 1.0 / sin_(theta);
}

float sec_(float theta){
	return 1.0 / cos_(theta);
}

float cot_(float theta){
	return cos_(theta) / sin_(theta);
}


float arcsin_(float z){
	int n = 0;
	float result = 0;

	if(f_abs_(z) <= 1) {
		for(n = 0; n < INF_SERIES_PRECISION; n++){
			result += (comb_(2*n, n)*pow_(z, 2*n + 1)) / (pow_(4.0, n)*(2*n + 1));
		}
	}
	else {
		return -1.0;
	}

	return result;
}

float arccos_(float z){
	if(f_abs_(z) <= 1) {
		return PI_/2 - arcsin_(z);
	}
	else {
		return -1.0;
	}
}

float arctan_(float z){
	int n = 0;
	float result = 0;

	if(f_abs_(z) <= 1) {
		for(n = 0; n < INF_SERIES_PRECISION; n++){
			result += (pow_(-1.0, n)*pow_(z, 2*n + 1)) / (2*n + 1);
		}
	}
	else {
		return -1.0;
	}

	return result;
}



// combinatorics
long comb_(unsigned int n, unsigned int k){
	return (long)((float)fact_(n) / (float)(fact_(k)*fact_(n - k))); // otherwise linking problem (__aeabi_ldiv0), need to fix 
}

long perm_(unsigned int n, unsigned int k){
	return (long)((float)fact_(n) / (float)fact_(n - k)); // otherwise linking problem (__aeabi_ldiv0), need to fix 
}



// vectors
float norm_(int x, int y){
	return sqrt_(x*x + y*y);
}



