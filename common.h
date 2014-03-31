#ifndef COMMON_H
#define COMMON_H

#include "spin1_api.h"
#include "spinn_io.h"
#include "spinn_sdp.h"

#include "math.h"

// simulation constants
#define PRINT_DLY          200
#define MS 1000
#define TICK_TIME (100*MS) // in ms
#define DELTA_TIME 0.1 // TICK_TIME in s
#define INV_DELTA_TIME 10
// end of simulation constants


typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#endif
