// author: Jonny Quarta
// Date: March 19st 2014

#include "BallController.h"


// ball tracking
uint x_buffer[16] = {0};
uint y_buffer[16] = {0};
uint total_sum_x = 0;
uint total_sum_y = 0;
uint curr_index = 0;
// end of ball tracking


// ball parameters
uint x_pos = 0;
uint y_pos = 0;
uint old_x_pos = 0;
uint old_y_pos = 0;
int x_speed = 0;
int y_speed = 0;
// end of ball parameters

// motors comands
int x_command = 0;
int y_command = 0;
// end of motors commands





// ball tracking

void compute_pos(uint x_cur, uint y_cur)
{
        total_sum_x = total_sum_x - x_buffer[curr_index] + x_cur;
        x_buffer[curr_index] = x_cur;
        total_sum_y = total_sum_y - y_buffer[curr_index] + y_cur;
        y_buffer[curr_index] = y_cur;

        x_pos = total_sum_x >> 4; // division by 16
        y_pos = total_sum_y >> 4; // division by 16


        curr_index = (curr_index + 1) % 16;
}

// call every tick (0.1s)
void compute_speed()
{
        x_speed = ((int)x_pos - (int)old_x_pos) * INV_DELTA_TIME;
        y_speed = ((int)y_pos - (int)old_y_pos) * INV_DELTA_TIME;


        old_x_pos = x_pos;
        old_y_pos = y_pos;
}

void cameraEvent(uint key, uint payload){
	// raw position extraction
	uint x_cur = ((key & 0x7F));
	uint y_cur = (((key >> 8) & 0x7F));
	int pol = (key >> 7) & 0x01;
	int check = (key >> 15) & 0x01;

	if(pol == 1) {
		compute_pos(x_cur, y_cur);
		//io_printf (IO_STD, "(%d, %d)\n", x_pos, y_pos);	
	}
}




void update(uint sim_time, uint none)
{
	// ball params update
	compute_speed();
	// end of ball params update

	// networks update
	updateError(x_pos, y_pos, x_speed, y_speed);
	update_V();
	// TODO: actor network updating 
	// TODO: actor network motor commands retrieval
	// end of networks update
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



	// events setting
	spin1_callback_on(MC_PACKET_RECEIVED,cameraEvent,1);
	spin1_callback_on(TIMER_TICK, update, 1);	
	
	if(chipID == 0) {
		initIO();
		if(coreID == 1) {
			startDevices();
		}
	}

	// end of events setting

	// networks init
	init_V(chipID, coreID);
	// TODO: actor network init
	// end of networks init
	
	spin1_start();


}


