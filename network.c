#include "network.h"

static float old_V = 0.5;
static float error = 0.0;
static uint x_pos_ = 0;
static uint y_pos_ = 0;
// TODO: has to change when parallelized
static float e_array[N_V] = {0.0};
static float w_V_array[N_V] = {0.0};
static uint center_x[N_V] = {63};
static uint center_y[N_V] = {63};

// used only for gaussian (unparallelized)
static float total_a = 0.0;

// reward
float r_pos() {
	// TODO: change from linear to other
	float d = norm_(x_pos_ - BALL_POS_CENTER, y_pos_ - BALL_POS_CENTER);
	return 1.0 - d/(BALL_POS_MAX - BALL_POS_MIN);
}

float r_tan(int x, int y){
	return (2.0 / PI_) * arctan_( (PI_/2.0) * norm_(x, y) );
}

float R(int x_speed, int y_speed, int x_acc, int y_acc){
	return r_pos() - r_tan(x_speed, y_speed) - r_tan(x_acc, y_acc);
}

void updateError(uint x_pos, uint y_pos, int x_speed, int y_speed, int x_acc, int y_acc) {
	x_pos_ = x_pos;
	y_pos_ = y_pos;
	float r = R(x_speed, y_speed, x_acc, y_acc);
	float curr_V = V();

	float v = (float)INV_DELTA_TIME * ( (1.0 - DELTA_TIME/TAU)*curr_V - old_V );
	error = r + v;

	old_V = curr_V;



	//io_printf(IO_STD,"error is %d, %d, %d \n", (int)(r*10000.0), (int)(v*10000.0), (int)(error*10000.0));
}



// critic network
void init_V(){
	// compute center position for each neuron
	
}


float V() {
	float v = 0;
	int i = 0;
	float w_i = 0;
	float phi_i = 0;
	
	// TODO: change into unparallelized (maybe not needed since MFM will be used)
	total_a = 0.0;
	for(i = 0; i < N_V; i++) {
		total_a += a_V(i);
	}


	for(i = 0; i < N_V; i++) {
		getInput_V(i, &w_i, &phi_i);
		v += w_i * phi_i;
	}

	return v;
}


// For the moment the parameters are unparalelized, but this will be the case in the future
void getInput_V(uint index, float* w, float* phi) {
	*w = w_V(index);
	*phi = phi_V(index);
} // TODO: change into parallelized



// Unparallelized parameters
float w_V(uint index){
	return w_V_array[index];
}

float phi_V(uint index){
	if(index == 0){
	io_printf(IO_STD,"a is %d, %d,  %d\n", x_pos_, y_pos_, (uint)(exp_(-4.0)*10000.0));
	}
	return a_V(index) / total_a;
}

float a_V(uint index){
	return exp_(-( pow_((x_pos_-center_x[index])/GAUSSIAN_SPREAD, 2) +  pow_((y_pos_-center_y[index])/GAUSSIAN_SPREAD, 2) ));
}


// Critic network updating
void update_V() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_V; i++) {
		e_array[i] = LAMBDA*e_array[i] + delta_V_wi(i);
		w_V_array[i] += LEARNING_RATE*error*e_array[i];
	}
}

float delta_V_wi(uint index) {
	return phi_V(index); // TODO: maybe has to change
}

