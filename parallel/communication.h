#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "common.h"

#define EAST 0x1
#define N_EAST 0x2
#define NORTH 0x4
#define WEST 0x8
#define S_WEST 0x10
#define SOUTH 0x20


#define CORE(n) (1<< (n+6))

#define ALL_NODES_MASTER_CHIP CORE(2)|CORE(3)|CORE(4)|CORE(5)|CORE(6)|CORE(7)|CORE(8)|CORE(9)|CORE(10)|CORE(11)|CORE(12)|CORE(13)|CORE(14)|CORE(15)
#define ALL_NODES_NO_MASTER_CHIP CORE(1)|ALL_NODES_MASTER_CHIP
#define MASTER_CORE CORE(1)


#define MGMT_BIT 0x400
#define EVDS1_XY 0x43
#define EVDS1_ENABLE 0x45
#define MOTORS_KEY ((252 << 24) | (255 << 16))

#define MOTOR_MIN 1500
#define MOTOR_MAX 2500
#define MOTOR_DELTA ((MOTOR_MAX - MOTOR_MIN) / 2)

#define MOTOR_X_CENTER	2150
#define MOTOR_Y_CENTER	2000//1970

#define ERROR_MSG (uint)(0xA0000000) // format - key [ A (4) - 0 (10) - sign (2) - x (8) - y (8)] payload : [ error_x (16) - error_y (16) ]
#define UPD_MSG (uint)(0x40000000) // format - key: [ 4 (2) - signs (4) - index (6) - v_x (10) - v_y (10)] payload : [theta (16) - psi (16)] Note that values are positive integers (v * 1000) and (theta, psi * 10000)
#define SAVE_MSG 52



void initIO(uint chipID, uint coreID);
void startDevices(uint chipID, uint coreID);
void stopDevices();

void sendNormMotorCommand(float x, float y);
void sendMotorCommand(uint motor_x, uint motor_y);


#endif
