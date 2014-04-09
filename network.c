#include "network.h"

void raise() {}

static float old_V = 0.0;
static float error = 0.0;
static vector2d pos_;
// TODO: has to change when parallelized
static float e_array[N_V] = {0.0};
static float w_V_array[N_V] = {0.0};
static float w_A_array[N_A] = {0.0};
static vector2d center[N_V];

// used only for gaussian (unparallelized)
static float total_a = 0.0;

// used only for mfm (unparallelized)
static long F[N_V] = {10};


// normalizes ball position and returns normalized ball speed
vector2d normalizeBallParams(int x_pos, int y_pos, int x_speed, int y_speed) {
	// speed normalization
	vector2d speed = empty_vector();
	vector2d int_sp = vector((float)x_speed, (float)y_speed);

	float speed_ratio = v_norm(int_sp) / (float)MAX_BALL_SPEED;

	if(speed_ratio > 1.0) {
		speed_ratio = 1.0;
	}

	speed = normalize(int_sp);
	speed = v_mul(speed_ratio, speed);


	// position normalization
	pos_.x = A_X*x_pos + B_X;
	pos_.y = A_Y*y_pos + B_Y;

	return speed;
}



// reward
float r_pos() {
	// TODO: change from linear to other
	return 1.0 - v_norm(pos_);
}


float R(vector2d speed){
	float r = r_pos() - (2.0 / PI_) * atan(v_norm(speed));
	
	if(r < 0.0){
		r = 0.0;
	} else if(r > 1.0) {
		r = 1.0;
	}

	return r;
}

void updateError(int x_pos, int y_pos, int x_speed, int y_speed) {
	vector2d speed = normalizeBallParams(x_pos, y_pos, x_speed, y_speed);
	float r = R(speed);
	float curr_V = V();

	float v = INV_DELTA_TIME * ( (1.0 - DELTA_TIME_OVER_TAU)*curr_V - old_V );
	error = r + v;

	old_V = curr_V;

	
	// TODO: (when parallelized) spread new error to every node
	
	io_printf(IO_STD,"error %d, (error + 1) %d\n", (int)(error*1000), (int)((error+1.0)*1000));
}



// critic network
// compute center position for each neuron (inner, has to change when parallelized)
void init_V_(uint chipID, uint coreID){
	pos_ = empty_vector();
	int chipX = chipID & 0x000000FF;
	int chipY = chipID >> 8;
	int coreX = (coreID & 0x00000003); // 0 to 3
	int coreY = (coreID >> 2); // 0 to 3

	float p1 = (float)coreX / 4.0;
	float p2 = (float)coreY / 4.0;

	if(p1 == 0.0) {
		p1 = 0.1;
	}

	float r = sqrt(p1);
	float theta = ((PI_/2.0) * p2) + ((PI_/2.0)*(chipX + 2.0*chipY)); // + base theta

	float x = r*cos(theta);
	float y = r*sin(theta);

	// TODO: has to change when parallelized
	center[32*chipY + 16*chipX + coreID].x = x;
	center[32*chipY + 16*chipX + coreID].y = y;
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
		v += w_V_array[i] * phi_V(i);
	}

	// TODO: (when parallelized) replacer multiplication by received values 
	return v;
}



// For the moment the parameters are unparalelized, but this will be the case in the future

float phi_V(int index){
	return 0.0;//mfm_V(index);
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

float mfm_V(int index){
	long summ = 0;
	// used if presynaptic MFM connections (not yet, TODO)
	/*int i=0;
	for(i=0; i<M; ++i){

		summ += (((J[i]*Fpre[i]) >> LOG_P1) );

	}*/

	long Iext = (long)(100*(1.0 - v_norm(v_sub(pos_, center[index]))));

	long dv = (( -F[index] + IFfct( summ + Iext ) ) * V_DECAY );

	F[index] = F[index] + dv;

	
	//io_printf(IO_STD,"f %d\n", F[index]);
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

float a_V(int index){
	return exp(-( pow((pos_.x-center[index].x)/GAUSSIAN_SPREAD, 2) +  pow((pos_.y-center[index].y)/GAUSSIAN_SPREAD, 2) ));
}




// Critic network updating
void update_V() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_V; i++) {
		e_array[i] = LAMBDA*e_array[i] + delta_V_wi(i);
		w_V_array[i] -= LEARNING_RATE_V*error*e_array[i];
	}

	// TODO: (when parallelized) remove loop and send new values (w_v * phi_V) to master node
}

float delta_V_wi(int index) {
	return phi_V(index); // TODO: maybe has to change
}




// Actor neural network
void move(int time) {
	int i = 0;
	float x_ = 0.0;
	float y_ = 0.0;
	vector2d n = noise(time);

	for(i = 0; i < N_A; i++) {
		if(i < N_A/2) { // x component
			x_ += w_A_array[i] * phi_A(i);
		}
		else { // y component
			y_ += w_A_array[i] * phi_A(i);
		}
	}

	x_ += sigma() * n.x;
	y_ += sigma() * n.y;


	// TODO: (when parallelized) replacer multiplication by received values 

	// TODO: perform motor coord extraction
	

	
	// Motor command sending
	//sendMotorCommand((uint)x_, (uint)y_);
}


void update_A(int time) { // TODO: has to change when it will be parallelized
	int i = 0;
	vector2d n = noise(time);
	
	for(i = 0; i < N_A; i++) {
		if(i < N_A/2) { // x component
			w_A_array[i] -= LEARNING_RATE_A*error*n.x*delta_A_wi(i);
		}
		else { // y component
			w_A_array[i] -= LEARNING_RATE_A*error*n.y*delta_A_wi(i);
		}

	}

	// TODO: (when parallelized) remove loop and send new values (w_a * phi_A) to master node
}

float delta_A_wi(int index) {
	return phi_A(index); // TODO: maybe has to change
}

float phi_A(int index){
	if(index < N_A/2) { // x component
		return pos_.x;
	}
	else { // y component
		return pos_.y;
	}
}

float sigma(){
	return SIGMA_0 * min(K, max(0.0, 1.0-V()));
}

vector2d noise(int t){
	vector2d r = empty_vector();
	return r; // TODO: implement Poisson noise
}


