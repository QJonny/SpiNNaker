#include "network.h"

float old_V = 0.0;
float error = 1.0;

vector2d pos_;
vector2d speed_;
vector2d old_pos_;

vector2d center[N_MFM];

// TODO: has to change when parallelized
float e_array[N_MFM] = {0.0};
float w_V_array[N_MFM] = {0.0};

float w_A_theta_array[N_MFM] = {0.0};
float w_A_psi_array[N_MFM] = {0.0};

float F[N_MFM] = {0.0};

vector2d n; // noise


// average params
float pos_buffer[4] = {1.0};
float speed_buffer[4] = {1.0};
float total_sum_pos = 4.0;
float total_sum_speed = 4.0;
float avg_pos = 1.0;
float avg_speed = 1.0;
int avg_index = 0;
// end of average params



// average pos and speed
void average_params() {
	float n_pos = v_norm(pos_);
	float n_sp = v_norm(speed_);

	total_sum_pos = total_sum_pos - pos_buffer[avg_index] + n_pos;
	total_sum_speed = total_sum_speed - speed_buffer[avg_index] + n_sp;
	pos_buffer[avg_index] = n_pos;
	speed_buffer[avg_index] = n_sp;

	avg_pos = total_sum_pos / 4.0;
	avg_speed = total_sum_speed / 4.0;
		
	avg_index = (avg_index + 1) % 4;
}


// call every tick (0.1s)
void compute_speed(uint sim_time)
{
	if(sim_time % SPEED_AVERAGER_STEP == 0) {
		speed_.x = (pos_.x - old_pos_.x) * INV_DELTA_TIME * INV_SPEED_AVERAGER_STEP * SPEED_RESIZE_FACTOR;
		speed_.y = (pos_.y - old_pos_.y) * INV_DELTA_TIME * INV_SPEED_AVERAGER_STEP * SPEED_RESIZE_FACTOR;

		old_pos_.x = pos_.x;
		old_pos_.y = pos_.y;
	}
}

// normalizes ball position and returns normalized ball speed
void normalizeBallParams(int x_pos, int y_pos, uint sim_time) {
	// position normalization
	pos_.x = A_X*x_pos + B_X;
	pos_.y = A_Y*y_pos + B_Y;


	// normalizing pos
	float n_pos = v_norm(pos_);
	if(n_pos > 1.0) {
		pos_.x /= n_pos;
		pos_.y /= n_pos;
	}

	compute_speed(sim_time);

	average_params();
}



// reward
float r_pos() {
	// TODO: change from linear to other
	return 1.0 - v_norm(pos_);
}


float R(vector2d speed){
	float r = r_pos() - (2.0 / PI_) * atan(v_norm(speed));

	r = range(r, 0.0, 1.0);

	return r;
}







// compute center position for each neuron (inner, has to change when parallelized)
void init_MFM(uint chipID, uint coreID){
	int chipX = chipID & 0x000000FF;
	int chipY = chipID >> 8;

	// Homogeneous disk
	int coreX = (coreID & 0x00000003); // 0 to 3
	int coreY = (coreID >> 2); // 0 to 3

	float p1 = (float)coreX / 4.0;
	float p2 = (float)coreY / 4.0;

	if(p1 == 0.0) {
		p1 = 0.09;
	}

	float r = 1.15*sqrt(p1 - 0.05);
	float theta = ((PI_/2.0) * p2) + ((PI_/2.0)*(chipX + 2.0*chipY)); // + base theta

	float x = r*cos(theta);
	float y = r*sin(theta);



	// archimedean spiral
	/*float a = 0.0;
	float b = 0.05;
	float coreNb = (float)(32*chipY + 16*chipX + coreID);
	float theta = (coreNb / 64.0) * 7.5 * PI_;

	float x = (a+b*theta)*cos(theta);
	float y = (a+b*theta)*sin(theta);*/


	// TODO: has to change when parallelized
	center[32*chipY + 16*chipX + coreID].x = x;
	center[32*chipY + 16*chipX + coreID].y = y;
}



void init_params_() {
	int i = 0;

	// uniform number in range [-0.1, 0.1]
	for(i = 0; i < N_MFM; i++) {
		w_V_array[i] = 	(((float)rand() / RAND_MAX) / 5.0) - 0.1; 

		w_A_theta_array[i] = (((float)rand() / RAND_MAX) / 5.0) - 0.1;
		w_A_psi_array[i] = (((float)rand() / RAND_MAX) / 5.0) - 0.1;
	}


	for(i = 0; i < 4; i++) {
		pos_buffer[i] = 1.0;
		speed_buffer[i] = 1.0;
	}

}


// compute center position for each neuron (outer, has to change when parallelized)
void init_network(uint chipID, uint coreID, uint noise_seed){
	pos_ = vector(-1.0, 0.0);
	old_pos_ = vector(-1.0, 0.0);
	speed_ = vector(0.0, 0.0);
	n = vector(0.0, 0.0);

	srand(noise_seed);

	coreID = 0;
	int i = 0;
	uint chipsID[4] = {0, 1, 256, 257}; // 0,0  0,1  1,0  1,1
	uint chipIndex = 0;
	
	for(chipIndex = 0; chipIndex < 4; chipIndex++) {
		chipID = chipsID[chipIndex];

		for(coreID = 0; coreID < 16; coreID++) {
			init_MFM(chipID, coreID);
		}
	}

	init_params_();
}




// saving (printing param code)
void save_() {/*
	int i = 0;
	int length = 0;
	char text[10000] = "";

	// e_array
	length += sprintf(text + length, "float e_array[N_MFM] = [ \n");

	for(i = 0; i < N_MFM; i++) {
		//length += sprintf(text + length, "\t %f,\n", e_array[i]);
	}
	length += sprintf(text + length, "];\n");

	
	// w_V_array
	length += sprintf(text + length, "float w_V_array[N_MFM] = [ \n");

	for(i = 0; i < N_MFM; i++) {
		//length += sprintf(text + length, "\t %f,\n", w_V_array[i]);
	}
	length += sprintf(text + length, "];\n");


	// w_A_theta_array
	length += sprintf(text + length, "float w_A_theta_array[N_MFM] = [ \n");

	for(i = 0; i < N_MFM; i++) {
		//length += sprintf(text + length, "\t %f,\n", w_A_theta_array[i]);
	}
	length += sprintf(text + length, "];\n");


	// w_A_psi_array
	length += sprintf(text + length, "float w_A_psi_array[N_MFM] = [ \n");

	for(i = 0; i < N_MFM; i++) {
		//length += sprintf(text + length, "\t %f,\n", w_A_psi_array[i]);
	}
	length += sprintf(text + length, "];\n");

	// F
	length += sprintf(text + length, "float F[N_MFM] = [ \n");

	for(i = 0; i < N_MFM; i++) {
		//length += sprintf(text + length, "\t %f,\n", F[i]);
	}
	length += sprintf(text + length, "];\n");

	//io_printf(IO_STD, text);*/
}





// MFM functions

float IF(float input){
	
	float output = 0.0;
	
	float thr1 = 0.8;
	float slope = 5.0;
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

void mfm_(){
	int index = 0;

	for(index = 0; index < N_MFM; index++) {
		// 0: ball far from population center, 1: ball close to population center
		float Iext = (2.0 - v_norm(v_sub(pos_, center[index]))) / 2.0;

		float dv = ( -F[index] + IF( Iext ) ) * V_DECAY ;

		F[index] += dv;
	}
}





// critic network

void updateError(int x_pos, int y_pos, uint sim_time) {
	normalizeBallParams(x_pos, y_pos, sim_time);
	float r = R(speed_);
	float curr_V = INV_DELTA_TIME*V();

	error = r + GAMMA*curr_V - old_V;

	old_V = curr_V;

		
	// TODO: (when parallelized) spread new error to every node

	//io_printf(IO_STD,"v %d, error %d\n", (int)(curr_V*1000), (int)(error*1000));
}




// V in [0; 1]
float V() {
	float v = 1.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += w_V_array[i] * phi_V(i);
	}

	// TODO: (when parallelized) replacer multiplication by received values
	return v/ N_MFM;
}


// Critic network updating
void update_V() { // TODO: has to change when it will be parallelized
	int i = 0;
	
	for(i = 0; i < N_MFM; i++) {
		e_array[i] = LAMBDA*GAMMA*e_array[i] + phi_V(i);
		w_V_array[i] += LEARNING_RATE_V*error*e_array[i];
	}

	// TODO: (when parallelized) remove loop and send new values (w_v * phi_V) to master node
}


// For the moment the parameters are unparallelized, but this will be the case in the future

float phi_V(int index){
	return F[index];
}









// Actor neural network
int move(uint sim_time) {
	int i = 0;
	float theta = 0.0;
	float psi = 0.0;

	for(i = 0; i < N_MFM; i++) {
		theta += w_A_theta_array[i] * phi_A(i);
		psi += w_A_psi_array[i] * phi_A(i);
	}

	float coef = 1.0;//0.75;


	// range: [-1;1]	
	theta = coef*(theta + sigma() * n.x);
	psi = coef*(psi + sigma() * n.y);




	// TODO: (when parallelized) replacer multiplication by received values 
	
	//io_printf(IO_STD,"theta %d, psi %d\n", (int)(theta*1000), (int)(psi*1000));


	theta = range(theta, -1.0, 1.0);
	psi = range(psi, -1.0, 1.0);

	if(avg_pos < 0.3 && avg_speed < 0.05) {
		sendNormMotorCommand(0.0, 0.0);
		return STATE_BALANCED; // ball balanced
	}
	else if(sim_time % RESET_STEP == 0) {
		if(v_norm(pos_) > 0.7){
			sendNormMotorCommand(pos_.x, pos_.y);
		}
	}
	else {	
		sendNormMotorCommand(theta, psi);
	}



	return STATE_UNBALANCED;
}

float phi_A(int index){
	return F[index];
}



void update_A() { // TODO: has to change when it will be parallelized
	int i = 0;
	n = noise();	

	for(i = 0; i < N_MFM; i++) {
		w_A_theta_array[i] -= LEARNING_RATE_A*error*n.x*phi_A(i);
		w_A_psi_array[i] -= LEARNING_RATE_A*error*n.y*phi_A(i);
	}

	// TODO: (when parallelized) remove loop and send new values (w_a * phi_A) to master node
}


float sigma(){
	return SIGMA_0 * min(K, max(0.0, 1.0-old_V));
}

// Gaussian noise (Box-Muller transform)
vector2d noise(){
	float U1 = (float)rand() / RAND_MAX; // uniform number in range [0, 1]
	float U2 = (float)rand() / RAND_MAX; // ||

	if(U1 == 0.0) {
		U1 = 0.000001;
	}

	float R = sqrt(-2.0* (-0.693147));// log not usable... TODO: has to solve
	float theta = 2.0*PI_*U2;

	vector2d r = v_mul(0.5, vector(R*cos(theta), R*sin(theta)));

	r.x = range(r.x, -1.0, 1.0);
	r.y = range(r.y, -1.0, 1.0);

	return r;
}


