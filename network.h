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

#define BALL_POS_MAX 127
#define BALL_POS_MIN 0
#define BALL_POS_CENTER 63
#define N_V 64 // number of neurons in the critic network
#define N_A 64 // number of neurons in the actor network
#define sigma_0 0.5

// reward
float r_pos();
float r_tan(int x, int y);
float R(int x_speed, int y_speed);
void updateError(uint x_pos, uint y_pos, int x_speed, int y_speed);


// critic network
void init_V(uint chipID, uint coreID);
float V(); // returns the critic network value


// unparallelized
float w_V(uint index); // unparallelized version
float phi_V(uint index); // corresponds to the feature vector (unparallelized version)


float mfm_V(uint index); // unparallelized, used for mean field model
long IFfct(long long input);

float a_V(uint index); // unparallelized, used for gaussian
int updateTotal_a();

// critic network updating
void update_V();
float delta_V_wi(uint index);


// actor network
void move(uint &theta, uint &phi); // perform the next movement (returns the values)
void update_A();
float delta_A_wi(uint index);
float phi_A(uint index); // corresponds to the feature vector (unparallelized version)
float sigma();
float noise();


#endif
