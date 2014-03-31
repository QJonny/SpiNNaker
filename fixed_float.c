#include "fixed_float.h"


fixed_float FIXED_FLOAT(int integer, float fraction){
	fixed_float r;
	
	//if(integer < 0) {
	//	integer = -integer;
	//	r.full = (integer<<DECIMAL_SIZE) + ((1<<DECIMAL_SIZE)*fraction);
	//	r.full = -r.full;
	//}
	//else {
		r.full = (integer<<DECIMAL_SIZE) + ((1<<DECIMAL_SIZE)*fraction);
	//}

	return r;
}



fixed_float ff_mul_(fixed_float a, fixed_float b)
{
	fixed_float r;
	long res = (long)a.full * (long)b.full;
	r.full = (res & 0x0000FFFFFFFF0000) >> DECIMAL_SIZE;
	return r;
}

fixed_float ff_div_(fixed_float a, fixed_float b)
{
	
}


fixed_float ff_add_(fixed_float a, fixed_float b) {
	fixed_float r;

	r.full = a.full + b.full;
	return r;
}

fixed_float ff_sub_(fixed_float a, fixed_float b) {
	fixed_float r;
	//int frac_init = b.full & 0x0000FFFF;	

	//r.full = a.full + FIXED_FLOAT((b.full >> DECIMAL_SIZE), (float)frac_init / (1 << DECIMAL_SIZE)).full;
	r.full = a.full - b.full;	

	return r;
}



fixed_float fi_add_(fixed_float a, int b) {
	fixed_float fb;
	fb = FIXED_FLOAT(b, 0.0);

	return ff_add_(a, fb);
}

fixed_float fi_sub_(fixed_float a, int b) {
	fixed_float fb;
	fb = FIXED_FLOAT(b, 0.0);

	return ff_sub_(a, fb);
}

fixed_float if_sub_(int a, fixed_float b) {
	fixed_float fa;
	fa = FIXED_FLOAT(a, 0.0);

	return ff_sub_(fa, b);
}

fixed_float fi_mul_(fixed_float a, int b) {
	fixed_float fb;
	fb = FIXED_FLOAT(b, 0.0);

	return ff_mul_(a, fb);
}

fixed_float fi_div_(fixed_float a, int b) {
	fixed_float fb;
	fb = FIXED_FLOAT(b, 0.0);

	return ff_div_(a, fb);
}

fixed_float if_div_(int a, fixed_float b) {
	fixed_float fa;
	fa = FIXED_FLOAT(a, 0.0);

	return ff_div_(fa, b);
}





void print_fixed_float(fixed_float f, int* integer, unsigned int* fraction) {

	int frac_init = f.full & 0x0000FFFF;

	*integer = f.full >> DECIMAL_SIZE;
	*fraction = (((float)frac_init / (1 << DECIMAL_SIZE))*1000000);
}








