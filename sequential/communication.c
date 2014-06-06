#include "communication.h"



void initIO() {
	spin1_set_mc_table_entry(0x2, 	MOTORS_KEY, 0xFFFF0000,	EAST);

	spin1_set_mc_table_entry(0x1, 	EVDS1_ENABLE | MGMT_BIT, 0xFFFFFFFF, EAST);
	spin1_set_mc_table_entry(0x4, 	0x41, 0xFFFFFFFF, WEST);

	// camera reception entry
	spin1_set_mc_table_entry(0x3, 	0x0,	0x0, CORE(1));
}


void startDevices() {
	spin1_send_mc_packet(MOTORS_KEY,MOTOR_Y_CENTER | (MOTOR_X_CENTER << 16 ), 1);

  	spin1_send_mc_packet(EVDS1_ENABLE | MGMT_BIT, 1, 1);
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






