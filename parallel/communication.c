#include "communication.h"



void initIO(uint chipID, uint coreID) {
	if(chipID == 0) { // 0, 0
		spin1_set_mc_table_entry(0x2, 	MOTORS_KEY, 0xFFFF0000,	EAST);

		spin1_set_mc_table_entry(0x1, 	EVDS1_ENABLE | MGMT_BIT, 0xFFFFFFFF, EAST);
		spin1_set_mc_table_entry(0x4, 	0x41, 0xFFFFFFFF, WEST);

		// from master to nodes
		spin1_set_mc_table_entry(0x6, 	ERROR_MSG,	 0xF0000000, NORTH|N_EAST|EAST|ALL_NODES_MASTER_CHIP);
		// froms nodes to master
		spin1_set_mc_table_entry(0x7, 	UPD_MSG,	 0xC0000000, MASTER_CORE);

		spin1_set_mc_table_entry(0x20, 	0x0,	0x0, MASTER_CORE); // default: camera event
	}	
	else if(chipID == 1) { // 0, 1
		spin1_set_mc_table_entry(0x6, 	ERROR_MSG,	 0xF0000000, ALL_NODES_NO_MASTER_CHIP); // to nodes
		spin1_set_mc_table_entry(0x7, 	UPD_MSG,	 0xC0000000, SOUTH); // to master
	}
	else if(chipID == 256) { // 1, 0
		spin1_set_mc_table_entry(0x6, 	ERROR_MSG,	 0xF0000000, ALL_NODES_NO_MASTER_CHIP); // to nodes
		spin1_set_mc_table_entry(0x7, 	UPD_MSG,	 0xC0000000, WEST); // to master
	}
	else if(chipID == 257) { // 1, 1
		spin1_set_mc_table_entry(0x6, 	ERROR_MSG,	 0xF0000000, ALL_NODES_NO_MASTER_CHIP); // to nodes
		spin1_set_mc_table_entry(0x7, 	UPD_MSG,	 0xC0000000, S_WEST); // to master
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
