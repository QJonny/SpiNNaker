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


#define MGMT_BIT 0x400
#define EVDS1_XY 0x43
#define EVDS1_ENABLE 0x45
#define MOTORS_KEY ((252 << 24) | (255 << 16))

#define MOTOR_MIN 1500
#define MOTOR_MAX 2500
#define MOTOR_DELTA ((MOTOR_MAX - MOTOR_MIN) / 2)

#define MOTOR_X_CENTER	2150
#define MOTOR_Y_CENTER	2000//1970

#define BALL_POS_MSG 47
#define ERROR_MSG 48
#define A_MSG 49
#define THETA_MSG 50
#define PSI_MSG 51
#define SAVE_MSG 52



void initIO(uint chipID, uint coreID);
void startDevices(uint chipID, uint coreID);
void stopDevices();

void sendNormMotorCommand(float x, float y);
void sendMotorCommand(uint motor_x, uint motor_y);

#endif
