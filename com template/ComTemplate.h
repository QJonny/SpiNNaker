#ifndef COMM_TEMPLATE_H
#define COMM_TEMPLATE_H

#include "spin1_api.h"
#include "spinn_io.h"
#include "spinn_sdp.h"

//void raise();

// simulation constants
#define PRINT_DLY          200
#define MS 1000
#define TICK_TIME (100*MS) // in ms


typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;



#define M 15

#define NUMBER_OF_XCHIPS 2
#define NUMBER_OF_YCHIPS 2


#define EAST 0x1
#define N_EAST 0x2
#define NORTH 0x4
#define WEST 0x8
#define S_WEST 0x10
#define SOUTH 0x20

#define CORE(n) (1<< (n+6))





uint core_map [ NUMBER_OF_XCHIPS ][ NUMBER_OF_YCHIPS ] =
{
	{0x3 , 0x3 } , // ( 0 , 0 ) , (0 , 1)
	{0x0 , 0x0 }   // ( 1 , 0 ) , (1 , 1)
};


uint coreID;
uint chipID;






#endif
