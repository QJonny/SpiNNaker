// author: Jonny Quarta
// Date: March 19st 2014

#include "MemTest.h"

#define MC_PAYLOAD 15

int val = 0;

void update(uint sim_time, uint none)
{
	val++;

        io_printf (IO_STD, "Val: %d\n", val);
}


void c_main (void)
{		
	// simulation initialization
	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();

	//spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);

	io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	spin1_set_timer_tick(TICK_TIME);
	// end of simulation initialization

        spin1_callback_on(TIMER_TICK, update, 1);

	spin1_start();
}


