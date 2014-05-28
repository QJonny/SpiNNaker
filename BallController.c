// author: Jonny Quarta
// Date: March 19st 2014

#include "BallController.h"


// ball tracking
int x_buffer[16] = {0};
int y_buffer[16] = {0};
int total_sum_x = 0;
int total_sum_y = 0;
int curr_index = 0;
int old_time = -1;
// end of ball tracking


// ball parameters
int x_pos = 0;
int y_pos = 0;
// end of ball parameters

// motors comands
int x_command = 0;
int y_command = 0;
// end of motors commands

int pos_computed = 0;

int state = STATE_UNBALANCED;


// ball tracking

void compute_pos(int x_cur, int y_cur, int sim_time)
{
	if((pos_computed == 0 || norm(x_pos - x_cur, y_pos - y_cur) < 100)) {

		total_sum_x = total_sum_x - x_buffer[curr_index] + x_cur;
		x_buffer[curr_index] = x_cur;
		total_sum_y = total_sum_y - y_buffer[curr_index] + y_cur;
		y_buffer[curr_index] = y_cur;

		x_pos = total_sum_x >> 4; // division by 16
		y_pos = total_sum_y >> 4; // division by 16

		if(pos_computed == 0 && curr_index == 15) {
			pos_computed = 1;
		}

		curr_index = (curr_index + 1) % 16;

		old_time = sim_time;
	}
}




void update(uint sim_time, uint none)
{
	if(state == STATE_UNBALANCED) {
		mfm_();
		// actor network updating and command sending
		update_A( sim_time );
		state = move( sim_time );

		// critic networks update
		updateError(x_pos, y_pos, sim_time);
		update_V();
		// end of networks update
	}

	// save step
	if(state == STATE_BALANCED || (state != STATE_SAVED && sim_time != 0 && sim_time % SAVE_STEP == 0)) {
		save_();
		state = STATE_SAVED;
		spin1_callback_off(TIMER_TICK);
	}
}




void cameraEvent(uint key, uint payload){
	// raw position extraction
	int y_cur = ((key & 0x7F));
	int x_cur = (((key >> 8) & 0x7F));
	int pol = (key >> 7) & 0x01;
	int check = (key >> 15) & 0x01;

	if(pol == 1 && spin1_get_simulation_time()) {
		compute_pos(x_cur, y_cur, spin1_get_simulation_time());
		io_printf(IO_STD,"camera event!\n");
	}
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
	

	initIO(chipID, coreID);
	startDevices(chipID, coreID);

	// end of events setting


	// networks init
	init_network(chipID, coreID, 0);

	
	// events setting
	spin1_callback_on(MC_PACKET_RECEIVED,cameraEvent,1);
	//spin1_callback_on(TIMER_TICK, update, 1);

	spin1_start();


}


