#ifndef FIXED_FLOAT_H
#define FIXED_FLOAT_H

#define INTEGER_SIZE 15 + 1 // number of bits for the integer part (+ 1 for sign). Integer part goes from +32768.xx to -32768.xx
#define DECIMAL_SIZE 16     // number of bits for the decimal part. Granularity is 0.000015259
#define FULL_SIZE_INT INTEGER_PART + DECIMAL_PART
#define FIXED_FLOAT_CHAR_SIZE 33
#define MAX_PRECISION 10


typedef union fixed_floatTag {
    int full; // used because of the special case 32
    struct partX_Ytag {
        short integer; // used because of the special case 16-16
        short fraction;
    } parts;
} fixed_float;


fixed_float FIXED_FLOAT(int integer, float fraction);


fixed_float ff_add_(fixed_float a, fixed_float b);
fixed_float ff_sub_(fixed_float a, fixed_float b);
fixed_float ff_mul_(fixed_float a, fixed_float b);
fixed_float ff_div_(fixed_float a, fixed_float b);

fixed_float fi_add_(fixed_float a, int b);
fixed_float fi_sub_(fixed_float a, int b);
fixed_float if_sub_(int a, fixed_float b);
fixed_float fi_mul_(fixed_float a, int b);
fixed_float fi_div_(fixed_float a, int b);
fixed_float if_div_(int a, fixed_float b);

void print_fixed_float(fixed_float f, int* integer, unsigned int* fraction);

#endif
