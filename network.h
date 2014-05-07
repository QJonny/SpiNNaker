#ifndef NETWORK_H
#define NETWORK_H


#include "common.h"
#include "communication.h"

// have been tuned for DELTA_TIME = 0.1
#define TAU 0.101
#define KAPPA 0.101
#define GAMMA 0.01
#define LAMBDA 1.0//(1.0 - DELTA_TIME/KAPPA) / (1.0 - DELTA_TIME/TAU)

#define LEARNING_RATE_V 0.5
#define LEARNING_RATE_A 0.5


#define LOG_P1 16
#define V_DECAY 0.5

#define BALL_POS_Y_MAX 115
#define BALL_POS_Y_MIN 5
#define BALL_POS_Y_CENTER (int)((BALL_POS_Y_MAX + BALL_POS_Y_MIN) / 2)

#define BALL_POS_X_MAX 125
#define BALL_POS_X_MIN 20
#define BALL_POS_X_CENTER (int)((BALL_POS_X_MAX + BALL_POS_X_MIN) / 2)


#define A_X -1.0/(BALL_POS_X_MAX - BALL_POS_X_CENTER)
#define B_X -(A_X)*BALL_POS_X_CENTER
#define A_Y -1.0/(BALL_POS_Y_MIN - BALL_POS_Y_CENTER)
#define B_Y -(A_Y)*BALL_POS_Y_CENTER


#define N_V 64 // number of neurons in the critic network
#define N_A 64 // number of neurons in the actor network
#define K 0.5
#define SIGMA_0 0.02


#define SPEED_AVERAGER_STEP 10
#define INV_SPEED_AVERAGER_STEP (1.0 / SPEED_AVERAGER_STEP)
#define RESET_STEP 30000


// reward
float r_pos();
float R(vector2d speed);
void updateError(int x_pos, int y_pos, uint sim_time);


// critic network
void init_V(uint chipID, uint coreID);
float V(); // returns the critic network value


// unparallelized
float phi_V(int index); // corresponds to the feature vector (unparallelized version)


float mfm_V(int index); // unparallelized, used for mean field model


// critic network updating
void update_V();
float delta_V_wi(int index);


// actor network
void init_A(uint noise_seed);
void move(uint sim_time); // perform the next movement
void update_A();
float delta_A_wi(int index);
float phi_A(int index); // corresponds to the feature vector (unparallelized version)
float sigma();
vector2d noise();


#endif
