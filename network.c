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

vector2d center[N_MFM];

float e_x_array[N_MFM] = {0.0};
float e_y_array[N_MFM] = {0.0};
float w_V_x_array[N_MFM] = {0.0};
float w_V_y_array[N_MFM] = {0.0};

float w_A_theta_array[N_MFM] = {0.0};
float w_A_psi_array[N_MFM] = {0.0};

float F_x[N_MFM] = {0.0};
float F_y[N_MFM] = {0.0};

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
void load_() {/*
w_V_x_array[0] = 477.0/1000.0;
w_V_x_array[1] = 645.0/1000.0;
w_V_x_array[2] = 730.0/1000.0;
w_V_x_array[3] = 548.0/1000.0;
w_V_x_array[4] = 454.0/1000.0;
w_V_x_array[5] = 663.0/1000.0;
w_V_x_array[6] = 609.0/1000.0;
w_V_x_array[7] = 739.0/1000.0;
w_V_x_array[8] = 404.0/1000.0;
w_V_x_array[9] = 737.0/1000.0;
w_V_x_array[10] = 578.0/1000.0;
w_V_x_array[11] = 580.0/1000.0;
w_V_x_array[12] = -146.0/1000.0;
w_V_x_array[13] = 427.0/1000.0;
w_V_x_array[14] = 503.0/1000.0;
w_V_x_array[15] = 671.0/1000.0;
w_V_x_array[16] = -456.0/1000.0;
w_V_x_array[17] = -403.0/1000.0;
w_V_x_array[18] = -430.0/1000.0;
w_V_x_array[19] = -502.0/1000.0;
w_V_x_array[20] = -185.0/1000.0;
w_V_x_array[21] = -5.0/1000.0;
w_V_x_array[22] = -127.0/1000.0;
w_V_x_array[23] = -92.0/1000.0;
w_V_x_array[24] = -39.0/1000.0;
w_V_x_array[25] = -209.0/1000.0;
w_V_x_array[26] = -271.0/1000.0;
w_V_x_array[27] = 19.0/1000.0;
w_V_x_array[28] = -152.0/1000.0;
w_V_x_array[29] = -115.0/1000.0;
w_V_x_array[30] = 50.0/1000.0;
w_V_x_array[31] = 18.0/1000.0;
w_V_x_array[32] = -172.0/1000.0;
w_V_x_array[33] = -184.0/1000.0;
w_V_x_array[34] = 219.0/1000.0;
w_V_x_array[35] = 137.0/1000.0;
w_V_x_array[36] = -104.0/1000.0;
w_V_x_array[37] = -121.0/1000.0;
w_V_x_array[38] = -105.0/1000.0;
w_V_x_array[39] = 8.0/1000.0;
w_V_x_array[40] = -26.0/1000.0;
w_V_x_array[41] = -126.0/1000.0;
w_V_x_array[42] = -214.0/1000.0;
w_V_x_array[43] = 34.0/1000.0;
w_V_x_array[44] = -171.0/1000.0;
w_V_x_array[45] = -75.0/1000.0;
w_V_x_array[46] = -42.0/1000.0;
w_V_x_array[47] = -96.0/1000.0;
w_V_x_array[48] = -340.0/1000.0;
w_V_x_array[49] = -419.0/1000.0;
w_V_x_array[50] = -469.0/1000.0;
w_V_x_array[51] = -398.0/1000.0;
w_V_x_array[52] = -161.0/1000.0;
w_V_x_array[53] = 314.0/1000.0;
w_V_x_array[54] = 389.0/1000.0;
w_V_x_array[55] = 751.0/1000.0;
w_V_x_array[56] = 304.0/1000.0;
w_V_x_array[57] = 690.0/1000.0;
w_V_x_array[58] = 458.0/1000.0;
w_V_x_array[59] = 715.0/1000.0;
w_V_x_array[60] = 351.0/1000.0;
w_V_x_array[61] = 528.0/1000.0;
w_V_x_array[62] = 757.0/1000.0;
w_V_x_array[63] = 603.0/1000.0;
w_V_y_array[0] = 49.0/1000.0;
w_V_y_array[1] = -75.0/1000.0;
w_V_y_array[2] = -42.0/1000.0;
w_V_y_array[3] = 15.0/1000.0;
w_V_y_array[4] = -35.0/1000.0;
w_V_y_array[5] = 225.0/1000.0;
w_V_y_array[6] = 502.0/1000.0;
w_V_y_array[7] = 887.0/1000.0;
w_V_y_array[8] = 118.0/1000.0;
w_V_y_array[9] = 768.0/1000.0;
w_V_y_array[10] = 587.0/1000.0;
w_V_y_array[11] = 443.0/1000.0;
w_V_y_array[12] = 258.0/1000.0;
w_V_y_array[13] = 840.0/1000.0;
w_V_y_array[14] = 300.0/1000.0;
w_V_y_array[15] = 143.0/1000.0;
w_V_y_array[16] = 295.0/1000.0;
w_V_y_array[17] = 794.0/1000.0;
w_V_y_array[18] = 133.0/1000.0;
w_V_y_array[19] = 54.0/1000.0;
w_V_y_array[20] = 306.0/1000.0;
w_V_y_array[21] = 875.0/1000.0;
w_V_y_array[22] = 358.0/1000.0;
w_V_y_array[23] = 142.0/1000.0;
w_V_y_array[24] = 152.0/1000.0;
w_V_y_array[25] = 784.0/1000.0;
w_V_y_array[26] = 635.0/1000.0;
w_V_y_array[27] = 317.0/1000.0;
w_V_y_array[28] = -110.0/1000.0;
w_V_y_array[29] = 229.0/1000.0;
w_V_y_array[30] = 597.0/1000.0;
w_V_y_array[31] = 874.0/1000.0;
w_V_y_array[32] = 19.0/1000.0;
w_V_y_array[33] = 3.0/1000.0;
w_V_y_array[34] = -22.0/1000.0;
w_V_y_array[35] = -105.0/1000.0;
w_V_y_array[36] = 276.0/1000.0;
w_V_y_array[37] = 505.0/1000.0;
w_V_y_array[38] = 762.0/1000.0;
w_V_y_array[39] = 632.0/1000.0;
w_V_y_array[40] = 443.0/1000.0;
w_V_y_array[41] = 571.0/1000.0;
w_V_y_array[42] = 878.0/1000.0;
w_V_y_array[43] = 690.0/1000.0;
w_V_y_array[44] = 594.0/1000.0;
w_V_y_array[45] = 701.0/1000.0;
w_V_y_array[46] = 794.0/1000.0;
w_V_y_array[47] = 260.0/1000.0;
w_V_y_array[48] = 599.0/1000.0;
w_V_y_array[49] = 804.0/1000.0;
w_V_y_array[50] = 802.0/1000.0;
w_V_y_array[51] = -36.0/1000.0;
w_V_y_array[52] = 546.0/1000.0;
w_V_y_array[53] = 675.0/1000.0;
w_V_y_array[54] = 776.0/1000.0;
w_V_y_array[55] = 230.0/1000.0;
w_V_y_array[56] = 385.0/1000.0;
w_V_y_array[57] = 564.0/1000.0;
w_V_y_array[58] = 799.0/1000.0;
w_V_y_array[59] = 656.0/1000.0;
w_V_y_array[60] = 354.0/1000.0;
w_V_y_array[61] = 601.0/1000.0;
w_V_y_array[62] = 663.0/1000.0;
w_V_y_array[63] = 652.0/1000.0;
w_A_theta_array[0] = -204.0/1000.0;
w_A_theta_array[1] = -503.0/1000.0;
w_A_theta_array[2] = -425.0/1000.0;
w_A_theta_array[3] = -140.0/1000.0;
w_A_theta_array[4] = -295.0/1000.0;
w_A_theta_array[5] = -465.0/1000.0;
w_A_theta_array[6] = -541.0/1000.0;
w_A_theta_array[7] = -179.0/1000.0;
w_A_theta_array[8] = -276.0/1000.0;
w_A_theta_array[9] = -355.0/1000.0;
w_A_theta_array[10] = -474.0/1000.0;
w_A_theta_array[11] = -555.0/1000.0;
w_A_theta_array[12] = -117.0/1000.0;
w_A_theta_array[13] = -302.0/1000.0;
w_A_theta_array[14] = -194.0/1000.0;
w_A_theta_array[15] = -286.0/1000.0;
w_A_theta_array[16] = -18.0/1000.0;
w_A_theta_array[17] = 123.0/1000.0;
w_A_theta_array[18] = -24.0/1000.0;
w_A_theta_array[19] = -30.0/1000.0;
w_A_theta_array[20] = 158.0/1000.0;
w_A_theta_array[21] = 176.0/1000.0;
w_A_theta_array[22] = 252.0/1000.0;
w_A_theta_array[23] = 105.0/1000.0;
w_A_theta_array[24] = 155.0/1000.0;
w_A_theta_array[25] = 59.0/1000.0;
w_A_theta_array[26] = 250.0/1000.0;
w_A_theta_array[27] = 303.0/1000.0;
w_A_theta_array[28] = 195.0/1000.0;
w_A_theta_array[29] = 90.0/1000.0;
w_A_theta_array[30] = 313.0/1000.0;
w_A_theta_array[31] = 338.0/1000.0;
w_A_theta_array[32] = 295.0/1000.0;
w_A_theta_array[33] = 94.0/1000.0;
w_A_theta_array[34] = 294.0/1000.0;
w_A_theta_array[35] = 280.0/1000.0;
w_A_theta_array[36] = 177.0/1000.0;
w_A_theta_array[37] = 102.0/1000.0;
w_A_theta_array[38] = 279.0/1000.0;
w_A_theta_array[39] = 337.0/1000.0;
w_A_theta_array[40] = 223.0/1000.0;
w_A_theta_array[41] = 99.0/1000.0;
w_A_theta_array[42] = 229.0/1000.0;
w_A_theta_array[43] = 346.0/1000.0;
w_A_theta_array[44] = 90.0/1000.0;
w_A_theta_array[45] = 112.0/1000.0;
w_A_theta_array[46] = 242.0/1000.0;
w_A_theta_array[47] = 71.0/1000.0;
w_A_theta_array[48] = 52.0/1000.0;
w_A_theta_array[49] = 41.0/1000.0;
w_A_theta_array[50] = 74.0/1000.0;
w_A_theta_array[51] = 115.0/1000.0;
w_A_theta_array[52] = -97.0/1000.0;
w_A_theta_array[53] = -162.0/1000.0;
w_A_theta_array[54] = -248.0/1000.0;
w_A_theta_array[55] = -377.0/1000.0;
w_A_theta_array[56] = -271.0/1000.0;
w_A_theta_array[57] = -301.0/1000.0;
w_A_theta_array[58] = -468.0/1000.0;
w_A_theta_array[59] = -668.0/1000.0;
w_A_theta_array[60] = -173.0/1000.0;
w_A_theta_array[61] = -383.0/1000.0;
w_A_theta_array[62] = -441.0/1000.0;
w_A_theta_array[63] = -211.0/1000.0;
w_A_psi_array[0] = 1075.0/1000.0;
w_A_psi_array[1] = 1020.0/1000.0;
w_A_psi_array[2] = 1110.0/1000.0;
w_A_psi_array[3] = 1192.0/1000.0;
w_A_psi_array[4] = 1314.0/1000.0;
w_A_psi_array[5] = 1197.0/1000.0;
w_A_psi_array[6] = 1031.0/1000.0;
w_A_psi_array[7] = 732.0/1000.0;
w_A_psi_array[8] = 1247.0/1000.0;
w_A_psi_array[9] = 761.0/1000.0;
w_A_psi_array[10] = 343.0/1000.0;
w_A_psi_array[11] = 233.0/1000.0;
w_A_psi_array[12] = 1024.0/1000.0;
w_A_psi_array[13] = 445.0/1000.0;
w_A_psi_array[14] = 161.0/1000.0;
w_A_psi_array[15] = 174.0/1000.0;
w_A_psi_array[16] = 1054.0/1000.0;
w_A_psi_array[17] = 338.0/1000.0;
w_A_psi_array[18] = 142.0/1000.0;
w_A_psi_array[19] = 75.0/1000.0;
w_A_psi_array[20] = 1150.0/1000.0;
w_A_psi_array[21] = 299.0/1000.0;
w_A_psi_array[22] = 143.0/1000.0;
w_A_psi_array[23] = 169.0/1000.0;
w_A_psi_array[24] = 1289.0/1000.0;
w_A_psi_array[25] = 829.0/1000.0;
w_A_psi_array[26] = 255.0/1000.0;
w_A_psi_array[27] = 210.0/1000.0;
w_A_psi_array[28] = 1238.0/1000.0;
w_A_psi_array[29] = 1088.0/1000.0;
w_A_psi_array[30] = 911.0/1000.0;
w_A_psi_array[31] = 718.0/1000.0;
w_A_psi_array[32] = 1069.0/1000.0;
w_A_psi_array[33] = 1165.0/1000.0;
w_A_psi_array[34] = 1157.0/1000.0;
w_A_psi_array[35] = 1025.0/1000.0;
w_A_psi_array[36] = 926.0/1000.0;
w_A_psi_array[37] = 972.0/1000.0;
w_A_psi_array[38] = 833.0/1000.0;
w_A_psi_array[39] = 966.0/1000.0;
w_A_psi_array[40] = 877.0/1000.0;
w_A_psi_array[41] = 941.0/1000.0;
w_A_psi_array[42] = 256.0/1000.0;
w_A_psi_array[43] = -327.0/1000.0;
w_A_psi_array[44] = 965.0/1000.0;
w_A_psi_array[45] = 611.0/1000.0;
w_A_psi_array[46] = -423.0/1000.0;
w_A_psi_array[47] = -759.0/1000.0;
w_A_psi_array[48] = 803.0/1000.0;
w_A_psi_array[49] = 494.0/1000.0;
w_A_psi_array[50] = -759.0/1000.0;
w_A_psi_array[51] = -527.0/1000.0;
w_A_psi_array[52] = 929.0/1000.0;
w_A_psi_array[53] = 649.0/1000.0;
w_A_psi_array[54] = -403.0/1000.0;
w_A_psi_array[55] = -686.0/1000.0;
w_A_psi_array[56] = 799.0/1000.0;
w_A_psi_array[57] = 890.0/1000.0;
w_A_psi_array[58] = 161.0/1000.0;
w_A_psi_array[59] = -361.0/1000.0;
w_A_psi_array[60] = 835.0/1000.0;
w_A_psi_array[61] = 989.0/1000.0;
w_A_psi_array[62] = 768.0/1000.0;
w_A_psi_array[63] = 830.0/1000.0;*/
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

		sp_x_term =  f_abs(speed_.x);
		sp_y_term =  f_abs(speed_.y);

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

	r = range(r, -1.0, 1.0);

	return r;
}

float R_y(){
	float r = r_pos_y() - sp_y_term;

	r = range(r, -1.0, 1.0);

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

	center[CORE_NB(chipY, chipX, coreID)].x = x;
	center[CORE_NB(chipY, chipX, coreID)].y = y;
}



void init_params_() {
	int i = 0;

	// uniform number in range [-0.1, 0.1]
	for(i = 0; i < N_MFM; i++) {
		w_V_x_array[i] = 	(((float)rand() / RAND_MAX) / 5.0) - 0.1; 
		w_V_y_array[i] = 	(((float)rand() / RAND_MAX) / 5.0) - 0.1; 

		w_A_theta_array[i] = (((float)rand() / RAND_MAX) / 5.0) - 0.1;
		w_A_psi_array[i] = (((float)rand() / RAND_MAX) / 5.0) - 0.1;
	}

	// should comment the previous loop and decomment this line for loading the parameters
	// Note that the SpiNNaker does not seem to accept a large function as the load_ one.
	// load_();

	for(i = 0; i < 4; i++) {
		pos_buffer[i] = 1.0;
		speed_buffer[i] = 1.0;
	}
}


// compute center position for each neuron (outer, has to change when parallelized)
void init_network(uint noise_seed){
	pos_ = vector(-1.0, 0.0);
	old_pos_ = vector(-1.0, 0.0);
	speed_ = vector(0.0, 0.0);
	n = vector(0.0, 0.0);

	srand(noise_seed);

	// we simulate a parallelization strategy in which every core
	// has a population (4 chips with 16 cores per chip)
	uint coreID = 0;
	uint chipID = 0;
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
void save_() {
	int i = 0;

	
	// w_V_array
	for(i = 0; i < N_MFM; i++) {
		io_printf(IO_STD, "w_V_x_array[%d] = %d.0/1000.0;\n", i, (int)(w_V_x_array[i]*1000));
	}

	for(i = 0; i < N_MFM; i++) {
		io_printf(IO_STD, "w_V_y_array[%d] = %d.0/1000.0;\n", i, (int)(w_V_y_array[i]*1000));
	}


	// w_A_theta_array
	for(i = 0; i < N_MFM; i++) {
		io_printf(IO_STD, "w_A_theta_array[%d] = %d.0/1000.0;\n", i, (int)(w_A_theta_array[i]*1000));
	}


	// w_A_psi_array
	for(i = 0; i < N_MFM; i++) {
		io_printf(IO_STD, "w_A_psi_array[%d] = %d.0/1000.0;\n", i, (int)(w_A_psi_array[i]*1000));
	}
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
	int index = 0;

	for(index = 0; index < N_MFM; index++) {
		// 0: ball far from population center, 1: ball close to population center
		vector2d distance =  v_sub(pos_, center[index]);
		float dist_x = f_abs(distance.x);
		float dist_y = f_abs(distance.y);

		float Iext = (2.0 - dist_x) / 2.0;

		float dv = ( -F_x[index] + IF( Iext ) ) * TAU_M;

		F_x[index] += dv;


		Iext = (2.0 - dist_y) / 2.0;

		dv = ( -F_y[index] + IF( Iext ) ) * TAU_M;

		F_y[index] += dv;
	}
}

float phi_MFM_x(int index) {
	return F_x[index];
}

float phi_MFM_y(int index) {
	return F_y[index];
}



// critic network

void updateError(int x_pos, int y_pos, uint sim_time) {
	normalizeBallParams(x_pos, y_pos, sim_time);
	
	float r = R_x();

	float curr_V = INV_DELTA_TIME * V_x();

	error_x = r + GAMMA*curr_V - old_V_x;

	old_V_x = curr_V;

	
	r = R_y();
	
	curr_V = INV_DELTA_TIME * V_y();

	error_y = r + GAMMA*curr_V - old_V_y;

	old_V_y = curr_V;


	//io_printf(IO_STD,"error x %d, error y %d\n", (int)(error_x*1000), (int)(error_y*1000));
}




// V in [0; 1]
float V_x() {
	float v = 0.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += w_V_x_array[i] * phi_MFM_x(i);
	}

	return v/ N_MFM;
}


// V in [0; 1]
float V_y() {
	float v = 0.0;
	int i = 0;

	for(i = 0; i < N_MFM; i++) {
		v += w_V_y_array[i] * phi_MFM_y(i);
	}

	return v/ N_MFM;
}



// Critic network updating
void update_V() {
	int i = 0;
	
	for(i = 0; i < N_MFM; i++) {
		e_x_array[i] = LAMBDA*GAMMA*e_x_array[i] + phi_MFM_x(i);
		w_V_x_array[i] += LEARNING_RATE_V*error_x*e_x_array[i];

		e_y_array[i] = LAMBDA*GAMMA*e_y_array[i] + phi_MFM_y(i);
		w_V_y_array[i] += LEARNING_RATE_V*error_y*e_y_array[i];
	}
}








// Actor neural network
int move(uint sim_time) {
	int i = 0;
	float theta = 0.0;
	float psi = 0.0;

	for(i = 0; i < N_MFM; i++) {
		theta += w_A_theta_array[i] * phi_MFM_x(i);
		psi += w_A_psi_array[i] * phi_MFM_y(i);
	}

	theta /= N_MFM;
	psi /= N_MFM;

	// range: [-1;1]	
	theta = theta + sigma_x() * n.x;
	psi = psi + sigma_y() * n.y;


	
	//io_printf(IO_STD,"theta %d, psi %d\n", (int)(theta*1000), (int)(psi*1000));


	theta = range(theta, -1.0, 1.0);
	psi = range(psi, -1.0, 1.0);

	if(sim_time > 300 && avg_pos < 0.2 && avg_speed < 0.05) {
		sendNormMotorCommand(0.0, 0.0);
		return STATE_BALANCED; // ball balanced
	}
	else if(sim_time % RESET_STEP == 0) {
		// send an impulse from time to time
		// if the ball is stucked on a place
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
	int i = 0;
	n = noise();	

	for(i = 0; i < N_MFM; i++) {
		w_A_theta_array[i] -= LEARNING_RATE_A*error_x*n.x*phi_MFM_x(i);
		w_A_psi_array[i] -= LEARNING_RATE_A*error_y*n.y*phi_MFM_y(i);
	}
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

	// multiplication by 0.5 in order to rescale the noise, so that
	// the following cut into the range [-1;1] does not eliminate
	// too much variance
	vector2d r = v_mul(0.5, vector(R*cos(theta), R*sin(theta)));

	r.x = range(r.x, -1.0, 1.0);
	r.y = range(r.y, -1.0, 1.0);

	return r;
}


