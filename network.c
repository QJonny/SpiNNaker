#include "network.h"

void raise() {}

static float old_V = 0.0;
static float error = 0.0;
static uint x_pos_ = 0;
static uint y_pos_ = 0;
// TODO: has to change when parallelized
static float e_array[N_V] = {0.0};
static float w_V_array[N_V] = {0.0};
static float w_A_array[N_A] = {0.0};
static uint center_x[N_V] = {63};
static uint center_y[N_V] = {63};

// used only for gaussian (unparallelized)
static float total_a = 0.0;

// used only for mfm (unparallelized)
static long F[N_V] = {10};



// reward
float r_pos() {
	// TODO: change from linear to other
	float d = norm(x_pos_ - BALL_POS_CENTER, y_pos_ - BALL_POS_CENTER);
	return 1.0 - d/(BALL_POS_MAX - BALL_POS_MIN);
}

float r_tan(int x, int y){
	return (2.0 / PI_) * atan( (PI_/500.0) * norm(x, y) );
}

float R(int x_speed, int y_speed){
	float r = r_pos() - r_tan(x_speed, y_speed);
	
	if(r < 0.0){
		r = 0.0;
	}

	return r;
}

void updateError(uint x_pos, uint y_pos, int x_speed, int y_speed) {
	x_pos_ = x_pos;
	y_pos_ = y_pos;
	float r = R(x_speed, y_speed);
	float curr_V = V();

	float v = INV_DELTA_TIME * ( (1.0 - DELTA_TIME_OVER_TAU)*curr_V - old_V );
	error = r + v;

	old_V = curr_V;


	// TODO: (when parallelized) spread new error to every node

	//io_printf(IO_STD,"error %d, r %d, V(t) %d\n", (int)(error*1000.0), (int)(r*1000.0), (int)(v*1000.0));
}



// critic network
// compute center position for each neuron (inner, has to change when parallelized)
void init_V_(uint chipID, uint coreID){
	uint chipX = chipID & 0x000000FF;
	uint chipY = chipID >> 8;
	uint coreX = (coreID & 0x00000003); // 0 to 3
	uint coreY = (coreID >> 2); // 0 to 3

	float p1 = (float)coreX / 4.0;
	float p2 = (float)coreY / 4.0;

	if(p1 == 0.0) {
		p1 = 0.1;
	}

	float r = sqrt(p1);
	float theta = ((PI_/2.0) * p2) + ((PI_/2.0)*(chipX + 2.0*chipY)); // + base theta

	float x = (r*cos(theta))*(BALL_POS_CENTER) + BALL_POS_CENTER;
	float y = (r*sin(theta))*(BALL_POS_CENTER) + BALL_POS_CENTER;

	// TODO: has to change when parallelized
	center_x[32*chipY + 16*chipX + coreID] = (uint)x;
	center_y[32*chipY + 16*chipX + coreID] = (uint)y;
}

// compute center position for each neuron (outer, has to change when parallelized)
void init_V(uint chipID, uint coreID){
	coreID = 0;
	int i = 0;	
	uint chipsID[4] = {0, 1, 256, 257}; // 0,0  0,1  1,0  1,1
	uint chipIndex = 0;
	
	for(chipIndex = 0; chipIndex < 4; chipIndex++) {
		chipID = chipsID[chipIndex];

		for(coreID = 0; coreID < 16; coreID++) {
			init_V_(chipID, coreID);
		}
	}
}


float V() {
	float v = 0;
	int i = 0;

	for(i = 0; i < N_V; i++) {
		v += w_V(i) * phi_V(i);
	}

	// TODO: (when parallelized) replacer multiplication by received values 

	return v;
}



// For the moment the parameters are unparalelized, but this will be the case in the future
float w_V(uint index){
	return w_V_array[index];
}

float phi_V(uint index){
	return mfm_V(index);
}



// MFM functions
long IFfct(long long input){
	
	long long output = 0;
	
	long long thr1 = 50 ;
	long long slope = 2;
	long long thr2 = 100 ;
	
	if(input < thr1){
		output = 0;
	}
	else{
		output = slope * (input - thr1);
	}
	
	if(output > thr2){
		output = thr2;
	}
	
	return output;
	
}

float mfm_V(uint index){
	long summ = 0;
	// used if presynaptic MFM connections (not yet, TODO)
	/*int i=0;
	for(i=0; i<M; ++i){

		summ += (((J[i]*Fpre[i]) >> LOG_P1) );

	}*/

	float d = norm(x_pos_ - center_x[index], y_pos_ - center_y[index]);
	long Iext = (long)(100*(1.0 - d/(BALL_POS_MAX - BALL_POS_MIN)));

	long dv = (( -F[index] + IFfct( summ + Iext ) ) * V_DECAY );

	F[index] = F[index] + dv;

	
	io_printf(IO_STD,"f %d\n", F[index]);
	return (float)F[index];
}



// gaussian functions
int updateTotal_a(){
	// TODO: change into unparallelized (maybe not needed since MFM will be used)
	total_a = 0.0;
	int i = 0;

	for(i = 0; i < N_V; i++) {
		total_a += a_V(i);
	}

	if(total_a == 0.0) {
		return 0;
	}

	return 1;
}

float a_V(uint index){
	return exp(-( pow((x_pos_-center_x[index])/GAUSSIAN_SPREAD, 2) +  pow((y_pos_-center_y[index])/GAUSSIAN_SPREAD, 2) ));
}




// Critic network updating
void update_V() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_V; i++) {
		e_array[i] = LAMBDA*e_array[i] + delta_V_wi(i);
		w_V_array[i] += LEARNING_RATE_V*error*e_array[i];
	}

	// TODO: (when parallelized) remove loop and send new values (w_v * phi_V) to master node
}

float delta_V_wi(uint index) {
	return phi_V(index); // TODO: maybe has to change
}




// Actor neural network
void move(uint &theta, uint &phi) {
	float v = 0;
	int i = 0;

	for(i = 0; i < N_V; i++) {
		v += w_A_array(i) * phi_A(i);
	}

	v += sigma()*noise();

	// TODO: (when parallelized) replacer multiplication by received values 

	// TODO: perform motor coord extraction
}


void update_A() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_A; i++) {
		w_A_array[i] += LEARNING_RATE_A*error*noise()*delta_A_wi(i);
	}

	// TODO: (when parallelized) remove loop and send new values (w_a * phi_A) to master node
}

float delta_A_wi(uint index) {
	return phi_A(index); // TODO: maybe has to change
}

float phi_A(uint index){
	return 0.0; // TODO: to change (into what??? ball position??)
}

float sigma(){
	return SIGMA_0 * min(K, max(0, 1-V()));
}

float noise(){
	return 0.0; // TODO: implement Poisson noise
}


