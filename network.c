#include "network.h"

float old_V = 0.0;
float error = 1.0;

vector2d pos_;
vector2d speed_;
vector2d old_pos_;

uint chipID;
uint coreID;

// master node
float V_array[N_MFM] = {0.0};

float A_theta_array[N_MFM] = {0.0};
float A_psi_array[N_MFM] = {0.0};


// slave nodes
float e = 0.0;
float w_V = 0.0;

float w_A_theta = 0.0;
float w_A_psi = 0.0;

float F = 0.0;
vector2d center;



vector2d n; // noise


// average params
float pos_buffer[4] = {0.0};
float speed_buffer[4] = {0.0};
float total_sum_pos = 4.0;
float total_sum_speed = 4.0;
float avg_pos = 1.0;
float avg_speed = 1.0;
int avg_index = 0;
// end of average params




// loading params
void load_() {

}





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
	return 1.0 - v_norm(pos_);
}


float R(vector2d speed){
	float r = r_pos() - (2.0 / PI_) * atan(v_norm(speed));

	r = range(r, 0.0, 1.0);

	return r;
}





// network communication updating
void updateNodePos(int x_pos, int y_pos, uint sim_time) {
	normalizeBallParams(x_pos, y_pos, sim_time);	
}

void updateNodeError(float err) {
	error = err;
}


void send_upd() {
	int chipX = chipID & 0x000000FF;
	int chipY = chipID >> 8;

	float v = w_V * phi_MFM();
	float theta = w_A_theta * phi_MFM();
	float psi = w_A_psi * phi_MFM();
	uint payload = 0;
	uint key = 0;

	// coding
	int index = CORE_NB(chipY, chipX, coreID);
	int iV = (int)(v * 10000.0);
	int iTheta = (int)(theta * 10000.0);
	int iPsi = (int)(psi * 10000.0);
	int signV = 0; // positive
	int signTheta = 0;
	int signPsi = 0;

	if (iV < 0) {
		signV = 1; // negative
		iV = -iV;
	}

	if (iTheta < 0) {
		signTheta = 1;
		iTheta = -iTheta;
	}

	if (iPsi < 0) {
		signPsi = 1;
		iPsi = -iPsi;
	}


	payload = (iTheta << 16) | iPsi;
	key = (0xA << 28) | (signV << 26) | (signTheta << 25) | (signPsi << 24) | (index << 16) | iV;

	// sending
	//spin1_send_mc_packet(key, payload, WITH_PAYLOAD);

}

void rec_upd(uint key, uint payload) {
	// deconding
	int iTheta = (payload >> 16);
	int iPsi = (payload & 0x0000FFFF);

	int iV = (key & 0x0000FFFF);
	int signV = (key >> 26) & 0x00000001;
	int signTheta = (key >> 25) & 0x00000001;
	int signPsi = (key >> 24) & 0x00000001;

	int index = (key >> 16) & 0x000000FF;

	if (signV == 1) {
		iV = -iV;
	}

	if (signTheta == 1) {
		iTheta = -iTheta;
	}

	if (signPsi == 1) {
		iPsi = -iPsi;
	}

	float v = (float)(iV) / 10000.0;
	float theta = (float)(iTheta) / 10000.0;
	float psi = (float)(iPsi) / 10000.0;

	//io_printf(IO_STD, "what the fuck???\n");

/*
	// updating
	V_array[index] = v;
	A_theta_array[index] = theta;
	A_psi_array[index] = psi;*/
}




// compute center position for each neuron
void init_MFM(){
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

	center.x = x;
	center.y = y;
}



void init_params_() {
	int i = 0;

	// uniform number in range [-0.1, 0.1]
	w_V = (((float)rand() / RAND_MAX) / 5.0) - 0.1; 

	w_A_theta = (((float)rand() / RAND_MAX) / 5.0) - 0.1;
	w_A_psi = (((float)rand() / RAND_MAX) / 5.0) - 0.1;

	//load_();

	if(chipID == 0 && coreID == 1) { // master core
		for(i = 0; i < 4; i++) {
			pos_buffer[i] = 1.0;
			speed_buffer[i] = 1.0;
		}
	}
}


// initialize network parameters
void init_network(uint chipIDs, uint coreIDs){
	chipID = chipIDs;
	coreID = coreIDs;
	pos_ = vector(-1.0, 0.0);
	old_pos_ = vector(-1.0, 0.0);
	speed_ = vector(0.0, 0.0);	
	n = vector(0.0, 0.0);

	int chipX = chipID & 0x000000FF;
	int chipY = chipID >> 8;

	srand(CORE_NB(chipY, chipX, coreID));

	init_MFM();

	init_params_();
}




// saving (printing param code)
void save_() {
	// e_array
	io_printf(IO_STD, "if(coreID==%d && chipID==%d){\n", coreID, chipID);
	io_printf(IO_STD, "\t e = %d.0/1000.0;\n", (int)(e*1000));

	io_printf(IO_STD, "\t w_V = %d.0/1000.0;\n", (int)(w_V*1000));

	io_printf(IO_STD, "\t w_A_theta = %d.0/1000.0;\n", (int)(w_A_theta*1000));

	io_printf(IO_STD, "\t w_A_psi = %d.0/1000.0;\n", (int)(w_A_psi*1000));
	io_printf(IO_STD, "}\n", coreID, chipID);

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
	// 0: ball far from population center, 1: ball close to population center
	float Iext = (2.0 - v_norm(v_sub(pos_, center))) / 2.0;

	float dv = ( -F + IF( Iext ) ) * V_DECAY ;

	F += dv;
}

float phi_MFM() {
	return F;
}





// critic network

void updateError() {
	float r = R(speed_);
	float curr_V = INV_DELTA_TIME*V();

	error = r + GAMMA*curr_V - old_V;

	old_V = curr_V;

	int* err = (int*)&error;

	spin1_send_mc_packet(ERROR_MSG, *err, WITH_PAYLOAD);
	//io_printf(IO_STD,"v %d, error %d\n", (int)(curr_V*1000), (int)(error*1000));
}




// V in [0; 1]
float V() {
	float v = 0.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += V_array[i];
	}

	return v/ N_MFM;
}


// Critic network updating
void update_V() {	
	e = LAMBDA*GAMMA*e + phi_MFM();
	w_V += LEARNING_RATE_V*error*e;
}








// Actor neural network
int move(uint sim_time) {
	int i = 0;
	float theta = 0.0;
	float psi = 0.0;

	for(i = 0; i < N_MFM; i++) {
		theta += A_theta_array[i];
		psi += A_psi_array[i];
	}

	float coef = 1.0;//0.75;


	// range: [-1;1]	
	theta = coef*(theta + sigma() * n.x);
	psi = coef*(psi + sigma() * n.y);


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


void update_A() {
	n = noise(); // generates the same noise because of the same seed between all nodes

	w_A_theta -= LEARNING_RATE_A*error*n.x*phi_MFM();
	w_A_psi -= LEARNING_RATE_A*error*n.y*phi_MFM();
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

	// log not usable... TODO: has to solve (just used a mean value of 0.5 so far, which still gives a noise impression)
	float R = sqrt(-2.0* (-0.693147));
	float theta = 2.0*PI_*U2;

	vector2d r = v_mul(0.5, vector(R*cos(theta), R*sin(theta)));

	r.x = range(r.x, -1.0, 1.0);
	r.y = range(r.y, -1.0, 1.0);

	return r;
}


