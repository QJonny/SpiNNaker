#ifndef NETWORK_H
#define NETWORK_H


#include "common.h"
#include "communication.h"

// ball parameters definitions
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

#define SPEED_RESIZE_FACTOR (float)(0.1)

// simulation definitions
#define SPEED_AVERAGER_STEP 5
#define INV_SPEED_AVERAGER_STEP (1.0 / SPEED_AVERAGER_STEP)
#define RESET_STEP 200 // in sec * 10
// end of ball parameters definitions





// Critic network definitions
// have been tuned for DELTA_TIME = 0.1
#define TAU 0.101
#define KAPPA 0.101
#define GAMMA 0.01
#define LAMBDA 1.0//(1.0 - DELTA_TIME/KAPPA) / (1.0 - DELTA_TIME/TAU)

#define LEARNING_RATE_V 0.5
// end of Critic network definitions


// MFM definitions
#define V_DECAY 0.5
#define N_MFM 64 // number of available populations
// end of MFM definitions



// Actor network definitions
#define LEARNING_RATE_A 0.5

#define K 0.5
#define SIGMA_0 0.1
// end of Actor network definitions




// Functions


// reward
float r_pos();
float R(vector2d speed);
void updateError(int x_pos, int y_pos, uint sim_time);


void init_network(uint chipID, uint coreID, uint noise_seed);


// mfm
void mfm_(); // unparallelized, used for mean field model
float phi_MFM(int index);

void save_();



// critic network
float V(); // returns the critic network value
void update_V();


// actor network
int move(uint sim_time); // perform the next movement
void update_A();

float sigma();
vector2d noise();


#endif
