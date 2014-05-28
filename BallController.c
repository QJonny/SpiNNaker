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
	if(updateCount == 1) {
		spin1_send_mc_packet(BALL_POS_MSG, (x_pos << 16)|y_pos, WITH_PAYLOAD);
	}
	else if(updateCount == 0){
		if(state == STATE_UNBALANCED) {
			// actor network command sending
			state = move( sim_count );

			// critic networks update
			updateError();

			// TODO: send error and noise
		}


		// save step
		if(state == STATE_BALANCED || (state != STATE_SAVED && sim_time != 0 && sim_time % SAVE_STEP == 0)) {
			save_();
			// TODO: send save message to nodes
			state = STATE_SAVED;
			spin1_callback_off(TIMER_TICK);
		}
	}

	updateCount = (updateCount + 1) % 4;
}




void event(uint key, uint payload){
	if(key == BALL_POS_MSG) {
		sim_count += 1;

		x_pos = payload >> 16;
		y_pos = payload & 0x0000FFFF;

		updateNodePos(x_pos, y_pos, sim_count);

		mfm_();
		// actor network update
		update_A( sim_count );

		// critic network update
		update_V();
	} // TODO: master: receive updatings
	else {  // camera event (why the hell isn't payload that contains the information????)
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

	//spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);

	io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	if(chipID == 0 && coreID == 1) { // master core
		spin1_set_timer_tick(TICK_TIME/4);
	}
	// end of simulation initialization	
	
	initIO(chipID, coreID);
	startDevices(chipID, coreID);


	// networks init
	init_network(chipID, coreID, 0);

	
	// events setting
	spin1_callback_on(MC_PACKET_RECEIVED,event,1);

	if(chipID == 0 && coreID == 1) { // master core
		spin1_callback_on(TIMER_TICK, update, 1);

	}

	srand(0); // up to now, every node generates the same noise! (no need to send it)

	spin1_start();
}


