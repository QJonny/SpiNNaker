#ifndef COMMON_H
#define COMMON_H

#include "stdlib.h"
#include "stdio.h"

#include "spin1_api.h"
#include "spinn_io.h"
#include "spinn_sdp.h"

#include "spin1_math.h"


void raise();

// simulation constants
#define PRINT_DLY 200
#define MS 1000
#define TICK_TIME (100*MS) // in ms
#define DELTA_TIME 0.1 // TICK_TIME in s
#define INV_DELTA_TIME (1.0 / DELTA_TIME)
// end of simulation constants

// state constants
#define STATE_UNBALANCED 0
#define STATE_BALANCED 1
#define STATE_SAVED 2
// end of state constants


typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#endif
