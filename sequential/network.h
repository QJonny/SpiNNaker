#ifndef NETWORK_H
#define NETWORK_H


#include "common.h"
#include "communication.h"


#define CORE_NB(chipY, chipX, coreID) (32*chipY + 16*chipX + (coreID - 1))

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
#define TAU_M 0.5
#define BETA 5.0
#define ALPHA 0.8
#define N_MFM 64 // number of available populations
// end of MFM definitions



// Actor network definitions
#define LEARNING_RATE_A 0.5

#define K 0.5
#define SIGMA_0 0.1
// end of Actor network definitions




// Functions


// reward
float r_pos_x();
float r_pos_y();
float R_x();
float R_y();
void updateError(int x_pos, int y_pos, uint sim_time);


void init_network(uint noise_seed);


// mfm
void mfm_();
float phi_MFM_x(int index);
float phi_MFM_y(int index);


// parameters handling
void save_();
void load_();



// critic network
float V_x(); // returns the critic network value along x
float V_y(); // returns the critic network value along y
void update_V();


// actor network
int move(uint sim_time); // performs the next movement
void update_A();

float sigma_x();
float sigma_y();
vector2d noise();


#endif
