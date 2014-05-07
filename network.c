#include "network.h"

void raise() {}

static float old_V = 0.0;
static float error = 1.0;

static vector2d pos_;
static vector2d speed_;
static vector2d old_pos_;

static vector2d center[N_V];

// TODO: has to change when parallelized
static float e_array[N_V] = {0.0};
static float w_V_array[N_V] = {0.0};
static float phi_V_old[N_V] = {0.0};

static float w_A_array[N_A] = {0.0};
static vector2d n; // noise


// used only for mfm (unparallelized)
static float F[N_V] = {10.0};


// call every tick (0.1s)
void compute_speed(uint sim_time)
{
	if(sim_time % SPEED_AVERAGER_STEP == 0) {
		speed_.x = (pos_.x - old_pos_.x) * INV_DELTA_TIME * INV_SPEED_AVERAGER_STEP;
		speed_.y = (pos_.y - old_pos_.y) * INV_DELTA_TIME * INV_SPEED_AVERAGER_STEP;

		old_pos_.x = pos_.x;
		old_pos_.y = pos_.y;
	}
}

// normalizes ball position and returns normalized ball speed
void normalizeBallParams(int x_pos, int y_pos, uint sim_time) {
	// position normalization
	pos_.x = A_X*x_pos + B_X;
	pos_.y = A_Y*y_pos + B_Y;

	compute_speed(sim_time);

	// speed normalization

	/*float speed_ratio = v_norm(int_sp) / (float)MAX_BALL_SPEED;

	if(speed_ratio > 1.0) {
		speed_ratio = 1.0;
	}

	speed = normalize(int_sp);
	speed = v_mul(speed_ratio, speed);*/

	pos_.x = range(pos_.x, 1.0, -1.0);
	pos_.y = range(pos_.y, 1.0, -1.0);
}



// reward
float r_pos() {
	// TODO: change from linear to other
	return 1.0 - v_norm(pos_);
}


float R(vector2d speed){
	float r = r_pos() - (2.0 / PI_) * atan(v_norm(speed));
	
	r = range(r, 1.0, 0.0);

	return r;
}


void updateError(int x_pos, int y_pos, uint sim_time) {
	normalizeBallParams(x_pos, y_pos, sim_time);
	float r = R(speed_);
	float curr_V = INV_DELTA_TIME*V();

	error = r + GAMMA*curr_V - old_V;

	old_V = curr_V;

		
	// TODO: (when parallelized) spread new error to every node

	io_printf(IO_STD,"v %d, error %d\n", (int)(curr_V*1000), (int)(error*1000));
}



// critic network
// compute center position for each neuron (inner, has to change when parallelized)
void init_V_(uint chipID, uint coreID){
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
	pos_ = vector(-1.0, 0.0);
	old_pos_ = vector(-1.0, 0.0);
	speed_ = vector(0.0, 0.0);

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

// V in [0; 1]
float V() {
	float v = 0;
	int i = 0;
	float phi = 0.0;

	for(i = 0; i < N_V; i++) {
		phi = phi_V(i);
		v += w_V_array[i] * phi;
		phi_V_old[i] = phi;
	}

	// TODO: (when parallelized) replacer multiplication by received values
	return v/ N_V;
}



// For the moment the parameters are unparalelized, but this will be the case in the future

float phi_V(int index){
	return mfm_V(index);//(2.0 - v_norm(v_sub(pos_, center[index])))/2.0;
}



// MFM functions

// From 0.5 to 1, slope 2
float IF(float input){
	
	float output = 0.0;
	
	float thr1 = 0.5;
	float slope = 2.0;
	float thr2 = 1.0;
	
	if(input < thr1){
		output = 0.0;
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
	float summ = 0;
	// used if presynaptic MFM connections (not yet, TODO)
	/*int i=0;
	for(i=0; i<M; ++i){

		summ += (((J[i]*Fpre[i]) >> LOG_P1) );

	}*/

	float Iext = (2.0 - v_norm(v_sub(pos_, center[index]))) / 2.0; // 0: ball far from population center, 1: ball close to population center

	float dv = ( -F[index] + IF( summ + Iext ) ) * V_DECAY ;

	F[index] += dv;

	
	//io_printf(IO_STD,"f %d\n", (int)(F[index]*1000));
	return F[index];
}




// Critic network updating
void update_V() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_V; i++) {
		// GAMMA*LAMBDA = -1
		e_array[i] = LAMBDA*GAMMA*e_array[i] + delta_V_wi(i);
		w_V_array[i] += LEARNING_RATE_V*error*e_array[i];
	}

	// TODO: (when parallelized) remove loop and send new values (w_v * phi_V) to master node
}

float delta_V_wi(int index) {
	return phi_V_old[index];//phi_V(index); // TODO: maybe has to change
}



















// Actor neural network
void init_A(uint noise_seed) {
	srand(noise_seed);
}



void move(uint sim_time) {
	int i = 0;
	float x_ = 0.0;
	float y_ = 0.0;

	for(i = 0; i < N_A; i++) {
		if(i < N_A/2) { // x component
			x_ += w_A_array[i] * phi_A(i);
		}
		else { // y component
			y_ += w_A_array[i] * phi_A(i);
		}
	}

	x_ /= N_A;
	y_ /= N_A;
	
	float coef = 1.0;//0.75;


	// range: [-1;1]	
	x_ = coef*(x_ + sigma() * n.x);
	y_ = coef*(y_ + sigma() * n.y);


	//x_ = -sin(pos_.x);//range(x_, 1.0, -1.0);
	//y_ = -sin(pos_.y);//range(y_, 1.0, -1.0);
	

	// TODO: (when parallelized) replacer multiplication by received values 
	
	//io_printf(IO_STD,"x %d, y %d\n", (int)(x_*1000), (int)(y_*1000));
	//io_printf(IO_STD,"speed %d, %d, norm %d\n", (int)(speed_.x*1000), (int)(speed_.y*1000), (int)(v_norm(speed_)*1000));
	
	// Motor command sending
	//sendNormMotorCommand(0.0, y_);
	//sendNormMotorCommand(x_, 0.0);

	if(sim_time % RESET_STEP == 0) {
		sendNormMotorCommand(-1.0, 0.0);
	}
	else {	
		sendNormMotorCommand(x_, y_);
	}
}


void update_A() { // TODO: has to change when it will be parallelized
	int i = 0;
	n = noise();
	
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
	return phi_A(index); 
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
	return SIGMA_0 * min(K, max(0.0, 1.0-old_V));
}

// Gaussian noise (Box-Muller transform)
vector2d noise(){
	float U1 = (float)rand() / RAND_MAX; // uniform number in range [0, 1]
	float U2 = (float)rand() / RAND_MAX; // ||

	float R = sqrt(-2.0*(logf(U1)/M_LOG10E));
	float theta = 2.0*PI_*U2;

	vector2d r = vector(R*cos(theta), R*sin(theta));
	return r;
}


