#ifndef NETWORK_H
#define NETWORK_H


#include "common.h"
#include "communication.h"

#define TAU 1
#define KAPPA 1
#define LAMBDA (1 - DELTA_TIME/KAPPA) / (1 - DELTA_TIME/TAU)
#define LEARNING_RATE_V 0.2
#define LEARNING_RATE_A 0.2
#define DELTA_TIME_OVER_TAU (DELTA_TIME / TAU)
#define K 0.5

#define GAUSSIAN_SPREAD 1.0

#define LOG_P1 16
#define V_DECAY (40)

#define BALL_POS_Y_MAX 115
#define BALL_POS_Y_MIN 5
#define BALL_POS_Y_CENTER (int)((BALL_POS_Y_MAX + BALL_POS_Y_MIN) / 2)

#define BALL_POS_X_MAX 125
#define BALL_POS_X_MIN 20
#define BALL_POS_X_CENTER (int)((BALL_POS_X_MAX + BALL_POS_X_MIN) / 2)

#define N_V 64 // number of neurons in the critic network
#define N_A 64 // number of neurons in the actor network
#define SIGMA_0 0.5

// reward
float r_pos();
float r_tan(int x, int y);
float R(int x_speed, int y_speed);
void updateError(int x_pos, int y_pos, int x_speed, int y_speed);


// critic network
void init_V(uint chipID, uint coreID);
float V(); // returns the critic network value


// unparallelized
float w_V(int index); // unparallelized version
float phi_V(int index); // corresponds to the feature vector (unparallelized version)


float mfm_V(int index); // unparallelized, used for mean field model
long IFfct(long long input);

float a_V(int index); // unparallelized, used for gaussian
int updateTotal_a();

// critic network updating
void update_V();
float delta_V_wi(int index);


// actor network
void move(int time); // perform the next movement
void update_A(int time);
float delta_A_wi(int index);
float phi_A(int index); // corresponds to the feature vector (unparallelized version)
float sigma();
vector2d noise(int t);


#endif
