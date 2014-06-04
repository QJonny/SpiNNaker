#include "network.h"

float old_V_x = 0.0;
float old_V_y = 0.0;
float error_x = 1.0;
float error_y = 1.0;

vector2d pos_;
vector2d speed_;
vector2d old_pos_;

float sp_x_term = 0.0;
float sp_y_term = 0.0;

uint chipID;
uint coreID;

// master node
float V_x_array[N_MFM] = {0.0};
float V_y_array[N_MFM] = {0.0};

float A_theta_array[N_MFM] = {0.0};
float A_psi_array[N_MFM] = {0.0};


// slave nodes
float e_x = 0.0;
float e_y = 0.0;
float w_V_x = 0.0;
float w_V_y = 0.0;

float w_A_theta = 0.0;
float w_A_psi = 0.0;

float F_x = 0.0;
float F_y = 0.0;
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

		sp_x_term =  0.5*f_abs(speed_.x);  //(2.0 / PI_) * atan(f_abs(speed_.x));
		sp_y_term =  0.5*f_abs(speed_.y);    //(2.0 / PI_) * atan(f_abs(speed_.y));

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
float r_pos_x() {
	return 1.0 - f_abs(pos_.x);
}

float r_pos_y() {
	return 1.0 - f_abs(pos_.y);
}


float R_x(){
	float r = r_pos_x() - sp_x_term;

	r = range(r, 0.0, 1.0);

	return r;
}

float R_y(){
	float r = r_pos_y() - sp_y_term;

	r = range(r, 0.0, 1.0);

	return r;
}





// network communication
void updateNodePos(int x_pos, int y_pos, uint sim_time) {
	
}

void updateNodeError(float err_x, float err_y) {
	error_x = err_x;
	error_y = err_y;
}


void send_err(uint x_pos, uint y_pos) {
	uint key = ERROR_MSG | (x_pos << 8) | y_pos;
	uint payload = 0;

	if(error_x < 0.0) {
		key = key | (1 << 17);
		payload = payload | ((uint)(-error_x*10000.0) << 16);
	} else {
		payload = payload | ((uint)(error_x*10000.0) << 16);
	}

	if(error_y < 0.0) {
		key = key | (1 << 16);
		payload = payload | (uint)(-error_y*10000.0);
	} else {
		payload = payload | (uint)(error_y*10000.0);
	}


	spin1_send_mc_packet(key, payload, WITH_PAYLOAD);

}


void rec_err(uint key, uint payload, uint sim_time) {
	uint x_pos = (key >> 8) & 0x000000FF;
	uint y_pos = key & 0x000000FF;

	normalizeBallParams(x_pos, y_pos, sim_time);


	// errors retrieving
	float err_x = 0.0;
	float err_y = 0.0;

	if((key >> 17) & 0x00000001 == 1) { // negative
		err_x = -((float)(payload >> 16)) / 10000.0;
	} else { // positive
		err_x = (float)(payload >> 16) / 10000.0;
	}		

	if((key >> 16) & 0x00000001 == 1) { // negative
		err_y = -((float)(payload & 0x0000FFFF)) / 10000.0;
	} else { // positive
		err_y = (float)(payload & 0x0000FFFF) / 10000.0;
	}

	error_x = err_x;
	error_y = err_y;
}






void send_upd() {
	int chipX = chipID & 0x000000FF;
	int chipY = chipID >> 8;

	uint key = UPD_MSG;
	uint payload = 0;


	float v_x = w_V_x * phi_MFM_x();
	float v_y = w_V_y * phi_MFM_y();
	float theta = w_A_theta * phi_MFM_x();
	float psi = w_A_psi * phi_MFM_y();

	v_x = range(v_x, -1.0, 1.0);
	v_y = range(v_y, -1.0, 1.0);
	theta = range(theta, -1.0, 1.0);
	psi = range(psi, -1.0, 1.0);

	// coding
	uint index = (uint)CORE_NB(chipY, chipX, coreID);

/*
	int iV = (int)(v_x * 10000.0);
	int iTheta = (int)(theta * 10000.0);
	int iPsi = (int)(psi * 10000.0);

	// check if negative
	if (iV < 0) {
		key += (uint)67108864; //1 << 26
		iV = -iV;
	}

	if (iTheta < 0) {
		key += (uint)33554432; // 1 << 25
		iTheta = -iTheta;
	}

	if (iPsi < 0) {
		key += (uint)16777216; // 1 << 24
		iPsi = -iPsi;
	}
	key += index * 65536; // index << 16

	key += (uint)iV;

	payload = ((uint)iTheta << 16) | (uint)iPsi;
*/
	//key = UPD_MSG | (signV << 26) | (signTheta << 25) | (signPsi << 24) | (index << 16) | (uint)iV; // does not work...


/*
	if(v_x < 0.0) {
		key = key | (1 << 29);
		key = key | (uint)(-v_x*1000.0);
	} else {
		key = key | (uint)(v_x*1000.0);
	}


	if(v_y < 0.0) {
		key = key | (1 << 28);
		key = key | ((uint)(-v_y*1000.0) << 10);
	} else {
		key = key | ((uint)(v_y*1000.0) << 10);
	}


	if(theta < 0.0) {
		key = key | (1 << 27);
		payload = payload | ((uint)(-theta*10000.0) << 16);
	} else {
		payload = payload | ((uint)(theta*10000.0) << 16);
	}

	if(psi < 0.0) {
		key = key | (1 << 26);
		payload = payload | (uint)(-psi*10000.0);
	} else {
		payload = payload | (uint)(psi*10000.0);
	}
	key = key | (index << 20);*/



	// sending
	spin1_send_mc_packet(key, payload, WITH_PAYLOAD);
	io_printf(IO_STD, "sent\n");
}

void rec_upd(uint key, uint payload) {/*
	// decoding
	int iTheta = (payload >> 16);
	int iPsi = (payload & 0x0000FFFF);


	int iV = key & 0x0000FFFF;
	uint signV = (key >> 26) & 0x00000001;
	uint signTheta = (key >> 25) & 0x00000001;
	uint signPsi = (key >> 24) & 0x00000001;

	uint index = (key >> 16) & 0x000000FF;


	float v = 1.0;//(float)iV;
	float theta = 0.5;//(float)(iTheta);
	float psi = 0.3;//(float)(iPsi);


	io_printf(IO_STD, "rec: %d\n", (int)v);

	if (signV == 1) {
		v = -v;
	}

	if (signTheta == 1) {
		theta = -theta;
	}

	if (signPsi == 1) {
		psi = -psi;
	}
*/
	// updating
//	V_array[index] = v;
/*	A_theta_array[index] = theta;
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
	w_V_x = (((float)rand() / RAND_MAX) / 5.0) - 0.1; 
	w_V_y = (((float)rand() / RAND_MAX) / 5.0) - 0.1; 

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

	srand(CORE_NB(chipY, chipX, coreID)+1);

	init_MFM();

	init_params_();
}




// saving (printing param code)
void save_() {
	// e_array
	io_printf(IO_STD, "if(coreID==%d && chipID==%d){\n", coreID, chipID);
	io_printf(IO_STD, "\t e_x = %d.0/1000.0;\n", (int)(e_x*1000));

	io_printf(IO_STD, "\t e_y = %d.0/1000.0;\n", (int)(e_y*1000));

	io_printf(IO_STD, "\t w_V_x = %d.0/1000.0;\n", (int)(w_V_x*1000));

	io_printf(IO_STD, "\t w_V_y = %d.0/1000.0;\n", (int)(w_V_y*1000));

	io_printf(IO_STD, "\t w_A_theta = %d.0/1000.0;\n", (int)(w_A_theta*1000));

	io_printf(IO_STD, "\t w_A_psi = %d.0/1000.0;\n", (int)(w_A_psi*1000));
	io_printf(IO_STD, "}\n", coreID, chipID);

}




// MFM functions

float IF(float input){
	
	float output = 0.0;
	
	float thr2 = 1.0;
	
	if(input < ALPHA){
		output = 0.0;
	}
	else{
		output = BETA * (input - ALPHA);
	}
	
	if(output > thr2){
		output = thr2;
	}
	
	return output;
}

void mfm_(){
	// 0: ball far from population center, 1: ball close to population center
	vector2d distance =  v_sub(pos_, center);
	float dist_x = f_abs(distance.x);
	float dist_y = f_abs(distance.y);

	float Iext = (2.0 - dist_x) / 2.0;

	float dv = ( -F_x + IF( Iext ) ) * TAU_M;

	F_x += dv;


	Iext = (2.0 - dist_y) / 2.0;

	dv = ( -F_y + IF( Iext ) ) * TAU_M;

	F_y += dv;
}

float phi_MFM_x() {
	return F_x;
}

float phi_MFM_y() {
	return F_y;
}





// critic network

void updateError(uint x_pos, uint y_pos, uint sim_time) {
	normalizeBallParams(x_pos, y_pos, sim_time);
	float r = R_x();

	float curr_V = INV_DELTA_TIME * V_x();

	error_x = r + GAMMA*curr_V - old_V_x;
	error_x = range(error_x, -1.0, 1.0);

	old_V_x = curr_V;

	
	r = R_y();
	
	curr_V = INV_DELTA_TIME * V_y();

	error_y = r + GAMMA*curr_V - old_V_y;
	error_y = range(error_y, -1.0, 1.0);

	old_V_y = curr_V;

	//io_printf(IO_STD,"updating\n");
}




// V in [0; 1]
float V_x() {
	float v = 0.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += V_x_array[i];
	}

	return v/ N_MFM;
}


// V in [0; 1]
float V_y() {
	float v = 0.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += V_y_array[i];
	}

	return v/ N_MFM;
}


// Critic network updating
void update_V() {	
	e_x = LAMBDA*GAMMA*e_x + phi_MFM_x();
	w_V_x += LEARNING_RATE_V*error_x*e_x;

	e_y = LAMBDA*GAMMA*e_y + phi_MFM_y();
	w_V_y += LEARNING_RATE_V*error_y*e_y;
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

	float coef = 0.1;//0.75;


	// range: [-1;1]	
	theta = coef*(theta + sigma_x() * n.x);
	psi = coef*(psi + sigma_y() * n.y);


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

	w_A_theta -= LEARNING_RATE_A*error_x*n.x*phi_MFM_x();
	w_A_psi -= LEARNING_RATE_A*error_y*n.y*phi_MFM_y();
}


float sigma_x(){
	return SIGMA_0 * min(K, max(0.0, 1.0-old_V_x));
}

float sigma_y(){
	return SIGMA_0 * min(K, max(0.0, 1.0-old_V_y));
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


