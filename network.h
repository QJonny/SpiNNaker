#ifndef NETWORK_H
#define NETWORK_H


#include "common.h"
#include "communication.h"

#define TAU 0.1
#define KAPPA 1
#define LAMBDA (1 - DELTA_TIME/KAPPA) / (1 - DELTA_TIME/TAU)
#define LEARNING_RATE 0.2
#define GAUSSIAN_SPREAD 1.0

#define BALL_POS_MAX 127
#define BALL_POS_MIN 0
#define BALL_POS_CENTER 63
#define N_V 100 // number of neurons in the critic network
#define N_A 100 // number of neurons in the actor network

// reward
float r_pos();
float r_tan(int x, int y);
float R(int x_speed, int y_speed, int x_acc, int y_acc);
void updateError(uint x_pos, uint y_pos, int x_speed, int y_speed, int x_acc, int y_acc);


// critic network
void init_V();
float V(); // returns the critic network value
void getInput_V(uint index, float* w, float* phi); // interface for getting network inputs for both unparallelized and parallelized versions


// unparallelized
float w_V(uint index); // unparallelized version
float phi_V(uint index); // corresponds to the feature vector (unparallelized version)

float a_V(uint index); // unparallelized, used for gaussian

// critic network updating
void update_V();
float delta_V_wi(uint index);

#endif
