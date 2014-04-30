#ifndef BALL_CONTROLLER_H
#define BALL_CONTROLLER_H

#include "common.h"
#include "communication.h"
#include "network.h"



#define M 15

#define NUMBER_OF_XCHIPS 2
#define NUMBER_OF_YCHIPS 2



uint core_map [ NUMBER_OF_XCHIPS ][ NUMBER_OF_YCHIPS ] =
{
	{0x3 , 0x3 } , // ( 0 , 0 ) , (0 , 1)
	{0x0 , 0x0 }   // ( 1 , 0 ) , (1 , 1)
};


uint coreID;
uint chipID;






#endif
