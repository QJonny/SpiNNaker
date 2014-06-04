// author: Jonny Quarta
// Date: March 19st 2014

#include "BallController.h"


// ball tracking
int x_buffer[16] = {0};
int y_buffer[16] = {0};
int total_sum_x = 0;
int total_sum_y = 0;
int curr_index = 0;
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

int updateCount = 0;
int sim_count = 1;

// ball tracking

void compute_pos(int x_cur, int y_cur)
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
	}
}




void update(uint sim_time, uint none)
{
	if(state == STATE_UNBALANCED) {
		// actor network command sending
		//state = move( sim_count );

		// critic networks update (also sends error and ball position)
		updateError(x_pos, y_pos, sim_time);
		send_err(x_pos, y_pos);
	}


	// save step
	if(state == STATE_BALANCED || (state != STATE_SAVED && sim_time != 0 && sim_time % SAVE_STEP == 0)) {
		save_();
		// TODO: send save message to nodes
		state = STATE_SAVED;
		spin1_callback_off(TIMER_TICK);
	}
}





void eventNode(uint key, uint payload){
	if((key & 0xF0000000) == ERROR_MSG) {
		sim_count += 1;

		rec_err(key, payload, sim_count);

		mfm_();

		// actor network update
		//update_A( sim_count );

		// critic network update
		update_V();

		send_upd();
	}
}



void eventMaster(uint key, uint payload){
	if((key & 0xC0000000) == UPD_MSG) {
		rec_upd(key, payload);
	} 
	else {
		// raw position extraction
		int y_cur = ((key & 0x7F));
		int x_cur = (((key >> 8) & 0x7F));
		int pol = (key >> 7) & 0x01;
		int check = (key >> 15) & 0x01;

		if(pol == 1 && spin1_get_simulation_time()) {
			compute_pos(x_cur, y_cur);
		}
	}
}



void c_main (void)
{		
	// simulation initialization
	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();



	if(chipID == 0 && coreID == 1) { // master core
		io_printf(IO_STD,"This is the master node\n");
		spin1_set_timer_tick(TICK_TIME);
	}
	// end of simulation initialization	
	
	initIO(chipID, coreID);
	startDevices(chipID, coreID);


	// networks init
	init_network(chipID, coreID);


	if(chipID == 0 && coreID == 1) { // master core
		// events setting
		spin1_callback_on(MC_PACKET_RECEIVED,eventMaster,0);
		spin1_callback_on(TIMER_TICK, update, 0);

	}
	else { // nodes
		// events setting
		spin1_callback_on(MC_PACKET_RECEIVED,eventNode,0);
	}

	srand(0); // up to now, every node generates the same noise! (no need to send it)

	spin1_start();
}


