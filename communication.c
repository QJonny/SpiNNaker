#include "communication.h"



void initIO(uint chipID, uint coreID) {
	if(chipID == 0) { // 0, 0
		spin1_set_mc_table_entry(0x2, 	MOTORS_KEY, 0xFFFF0000,	EAST);

		spin1_set_mc_table_entry(0x1, 	EVDS1_ENABLE | MGMT_BIT, 0xFFFFFFFF, EAST);
		spin1_set_mc_table_entry(0x4, 	0x41, 0xFFFFFFFF, WEST);

		// ball position
		spin1_set_mc_table_entry(0x5, 	BALL_POS_MSG,	 0xFFFFFFFF, NORTH|N_EAST|EAST);

		spin1_set_mc_table_entry(0x10, 	0x0,	0x0, CORE(1)); // default: camera event
	}	
	else if(chipID == 1) { // 0, 1
		spin1_set_mc_table_entry(0x2, 	BALL_POS_MSG,	 0xFFFFFFFF, CORE(1));
	}
	else if(chipID == 256) { // 1, 0
		spin1_set_mc_table_entry(0x2, 	BALL_POS_MSG,	 0xFFFFFFFF, CORE(1));
	}
	else if(chipID == 257) { // 1, 1
		spin1_set_mc_table_entry(0x2, 	BALL_POS_MSG,	 0xFFFFFFFF, CORE(1));
		//spin1_set_mc_table_entry(0x3, 	0x0,	0x0, CORE(1)|CORE(2)|CORE(3)|CORE(4)|CORE(5)|CORE(6)|CORE(7)|CORE(8)|CORE(9)|CORE(10)|CORE(11)|CORE(12)|CORE(13)|CORE(14)|CORE(15));
	}
}


void startDevices(uint chipID, uint coreID) {
	if(chipID == 0 && coreID == 1) { // master core
		spin1_send_mc_packet(MOTORS_KEY,MOTOR_Y_CENTER | (MOTOR_X_CENTER << 16 ), 1);

	  	spin1_send_mc_packet(EVDS1_ENABLE | MGMT_BIT, 1, 1);
	}
}



void stopDevices() {
  	spin1_send_mc_packet(EVDS1_ENABLE | MGMT_BIT, 0, 1);
}



void sendMotorCommand(uint motor_x, uint motor_y) {
	spin1_send_mc_packet(MOTORS_KEY, motor_y | (motor_x << 16 ), 1);
}


void sendNormMotorCommand(float x, float y) {
	sendMotorCommand((int)(x*MOTOR_DELTA) + MOTOR_X_CENTER, (int)(y*MOTOR_DELTA) + MOTOR_Y_CENTER);
}






/*
	sdp_msg_t* msg = spin1_msg_get();
	//msg->length = 36;

	msg->flags = 0x07;
	msg->tag = 0;
	msg->srce_port = (0x1<<5) | coreID;
	msg->dest_port = (0x1<<5) | coreID;
	msg->srce_addr = 0x0;
	msg->dest_addr = 0x1;

	msg->data[0] = (key & 0xFF000000) >> 24;
	msg->data[1] = (key & 0x00FF0000) >> 16;
	msg->data[2] = (key & 0x0000FF00) >> 8;
	msg->data[3] = key & 0x000000FF;

	spin1_send_sdp_msg(msg, 100);

	spin1_msg_free(msg);*/


/*
void sdp_rec(uint m, uint port){
	sdp_msg_t* msg = (sdp_msg_t*)m;

	uint key = ((uint)msg->data[0] << 24) | ((uint)msg->data[1] << 16) | ((uint)msg->data[2] << 8) | (uint)msg->data[3];
	x_cur = ((key & 0x7F));
	y_cur = (((key >> 8) & 0x7F));
	io_printf (IO_STD, "sdp rec (%d, %d)\n", x_cur, y_cur);
	io_printf (IO_STD, "sdp rec (%d)\n", port);

	spin1_msg_free(msg);	
}*/



	//if(chipID == 1) {
		//spin1_callback_on(SDP_PACKET_RX, sdp_rec, 2);
	//}
