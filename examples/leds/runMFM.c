//~ #define CORE1 0x80

#define NUMBER_OF_CHIPS    4
//~ #define CORE_MAP_00        0x0000e
#define CORE_MAP_00        0x00000
#define CORE_MAP_01        0x00000
#define CORE_MAP_10        0x1fffe
#define CORE_MAP_11        0x00000

#define EAST 0x1
#define N_EAST 0x2
#define NORTH 0x4
#define WEST 0x8
#define S_WEST 0x10
#define SOUTH 0x20


#include "spin1_api.h"
#include "spinn_io.h"
#include <stdint.h>

// scaling factors
#define LOG_P1                  (8)
#define LOG_P2                  (16)
#define P1						(1 << LOG_P1)
#define P2						(1 << LOG_P2)


// number of populations
#define M 	15


// plate angles
int theta, phi;

uint core_map[NUMBER_OF_CHIPS];
uint coreID;
uint chipID;


#define MOTOR_CMD_KEY		(1 << 31)
#define MAKE_PAYLOAD(X, Y) 	((X) << 16 | (uint16_t)(Y))
#define HAS_PAYLOAD			1
#define NO_PAYLOAD			0

// #define EXTRACT_X_PAYLOAD(P)

#define GET_X_COORD(KEY)	(((KEY) & 0xFF00) >> 8)
#define GET_Y_COORD(KEY)	((KEY) & 0xFF)


// firing rate
long F = 10 << LOG_P1;
//~ int F = 100;

// external current
long Iext;

// weight vector 
long J[M];

// presynaptic firing rate
long Fpre[M];

// synaptic efficacy
long U[M];

// synaptic resources
long x[M];


long Use[M];

long Tr[M];

long Tf[M];

long abs_(long n){
return n < 0 ? -n : n;
}

// calculating plate angles from output firing rates
// theta and phi are between -400 and 400
// (only from core M+1)
void send_mc_packet_angles() {	
	//~ theta = 10;
	//~ phi -= 10;
	
	theta = (int)((Fpre[9-1] - Fpre[ 7-1] )*4) >> LOG_P1;
	phi   = (int)((Fpre[8-1] - Fpre[10-1] )*4) >> LOG_P1;
	//~ int thetaF = (int)((Fpre[9-1] - Fpre[ 7-1] )*4) >> LOG_P1;
	//~ int phiF   = (int)((Fpre[8-1] - Fpre[10-1] )*4) >> LOG_P1;
	//~ phi = 0;
	//~ theta = -399;
	
	//~ if(thetaF>0){theta = 390;}
	//~ else{        theta =-390;}
	//~ if(phiF>0){phi = 390;}
	//~ else{      phi =-390;}
	
	//~ io_printf(IO_STD,"%d, %d\n", theta, phi);
	
	uint32_t payload = MAKE_PAYLOAD(theta, phi);
	
	spin1_send_mc_packet(MOTOR_CMD_KEY, payload, HAS_PAYLOAD);
	// io_printf(IO_STD,"sending packet ... %x \n", payload);
}




// IF curve
long IFfct(long long input){
	
	long long output = 0;
	
	long long thr1 = 50 << LOG_P1;
	//~ long long scf = 2 * 256;
	long long scf = 2 * 256;
	
	if(input < thr1){output = 0;}
	else{output = ((input - thr1) * scf ) >> LOG_P1;}
	
	if(output > (100 << LOG_P1)){output = 100 << LOG_P1;}
	
	return output;
	
}



void send_mc_packet_rates() {
	
	

	//~ io_printf(IO_STD,"----------------------\n");

	//~ long v_decay = 64 << LOG_P1;
    
    
    // 1/tau
    // (F/tau = -F + h())
	long v_decay = 32 << LOG_P1;
	//~ long v_decay = 2 << LOG_P1;


    // 1/tau_rec
	long x_decay = 10;
	
	//~ Iext = 50  << LOG_P1;
	//~ Iext = 100  << LOG_P1;
	//~ if (coreID == 1 ){Iext = 100  << LOG_P1;}
	//~ if (coreID == 2 ){Iext = 100  << LOG_P1;}
	//~ if (coreID == 3 ){Iext = 100  << LOG_P1;}
	//~ if (coreID == 4 ){Iext = 60  << LOG_P1;}
	//~ if (coreID == 5 ){Iext = 100  << LOG_P1;}
	
	
	//~ if (coreID == 1){Iext = 0 << LOG_P1;}
	//~ else{            Iext = 0  << LOG_P1;}
	
	//~ if(coreID == 4){Iext =  100 << LOG_P1;}
	
	
	
	long summ = 0;
	int i=0;
	for(i=0; i<M; ++i){
		//summ += ((J[i]*(256-(x[i]>>LOG_P1))*Fpre[i]) >> LOG_P2) ;
		//~ summ += ((J[i]*Fpre[i]) >> LOG_P1) * ((256 - x[i])>>LOG_P1);
		summ += (((J[i]*Fpre[i]) >> LOG_P1) );
		//~ summ += (((J[i]*Fpre[i]) >> LOG_P1) * ((256 - x[i])))>>LOG_P1;
        //long dx = (( - x[i]) * x_decay) >> LOG_P1;
		//int Add = 1*(10*(F>>LOG_P1))>>LOG_P1;
		//x[i] = x[i] + dx + Add;

		//~ x[i] = 0;
		//if(i==4-1 && coreID==9){ if(F>0){spin1_delay_us(coreID*100);io_printf(IO_STD,"x: %d, F: %d, J*Fpre: %d, Add: %d \n",(x[i]), F, IFfct( (J[i]*Fpre[i]) >> LOG_P1), Add); } }
	}
	long dF = (( - F + IFfct( summ + Iext ) ) * v_decay ) >> LOG_P1 ;
	dF = dF >> LOG_P1;
	
	
	//~ io_printf(IO_STD,"Dv %d\n", dv >> LOG_P1 );
	
	F = F + dF;

	//~ if(coreID==2){
		//~ spin1_delay_us(coreID*100);
		//~ io_printf(IO_STD,"Firing rate %d\n", F >> LOG_P1);
	//~ }
	
	
	//~ io_printf(IO_STD,"sending mc packet...\n");
	//~ spin1_send_mc_packet(0x1234, (uint)(F >> LOG_P1), 1);
	spin1_send_mc_packet(coreID, (uint)(F), 1);
	//~ spin1_send_mc_packet(0x1234, 0, 0);


	
}

void process_mc_packet(uint key, uint payload){
	//~ io_printf(IO_STD,"MC Packet received! key: %x\n",key);
		
	if(key>M+1){
        
        // update ball position
		uint x_ = GET_X_COORD(key);
		uint y_ = GET_Y_COORD(key);
		
		
		if(coreID==M+1){
			//~ int32_t t = ((int32_t)x - 64) * 6;
			//~ int32_t p = ((int32_t)y - 64) * 6;
			
			//~ theta = t;
			//~ phi = -p; // inverse sign for this direction

			//~ io_printf(IO_STD,"X: %3u, Y: %3u \n", x, y);

		}
				
		Iext = 50 << LOG_P1;
		//~ if((int32_t)x_<64 && coreID==2){Iext = -800 * (x_-64);}
		//~ if((int32_t)x_>64 && coreID==4){Iext =  800 * (x_-64);}
		//~ if((int32_t)y_<64 && coreID==3){Iext = -800 * (y_-64);}
		//~ if((int32_t)y_>64 && coreID==5){Iext =  800 * (y_-64);}
        
        // input populations receive external current as the result of the position x,y of the ball
		if((int32_t)x_<64 && coreID==2){Iext =  100 << LOG_P1;}
		if((int32_t)x_>64 && coreID==4){Iext =  100 << LOG_P1;}
		if((int32_t)y_<64 && coreID==3){Iext =  100 << LOG_P1;}
		if((int32_t)y_>64 && coreID==5){Iext =  100 << LOG_P1;}
		
		
	}
	else if(key<=M){
        
        // update presynaptic firing rate values
		Fpre[key-1] = payload;
	}
	
	//~ io_printf(IO_STD,"Payload: %d\n", Fpre[key-1] >> LOG_P1);
	//~ spin1_delay_us(100000);
	//~ io_printf(IO_STD,"Payload: %d\n", Fpre[key-1]);
}

int c_main() {
	int i=0;
	
	Iext = 0;

	if (NUMBER_OF_CHIPS > 0) core_map[0] = CORE_MAP_00;
	if (NUMBER_OF_CHIPS > 1) core_map[1] = CORE_MAP_01;
	if (NUMBER_OF_CHIPS > 2) core_map[2] = CORE_MAP_10;
	if (NUMBER_OF_CHIPS > 3) core_map[3] = CORE_MAP_11;

	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();
	spin1_delay_us(coreID*100);
	//~ io_printf(IO_STD," ================================ \n");
//~ 
	io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	//~ spin1_set_core_map(NUMBER_OF_CHIPS, core_map);
	
	//~ spin1_dselay_us(100000);
	
    
    // initialize connection variables
	for(i=0; i<M; ++i){
		Fpre[i] = 0;
		x[ i] = P1;
		U[ i] = 0;
		J[ i] = 0;
		Tr[i] = 64;
		Tf[i] = 64;
		Use[i]= 25; // 256 * 0.1 ?
	}
	
    
    // initialize weights
	if (coreID == 1 ){}
	if (coreID == 2 ){}
	if (coreID == 3 ){}
	if (coreID == 4 ){}
	if (coreID == 5 ){}
	if (coreID == 6 ){}
	if (coreID == 7 ){J[2-1] = 256;}
	if (coreID == 8 ){J[3-1] = 256;}
	if (coreID == 9 ){J[4-1] = 256;}
	if (coreID == 10){J[5-1] = 256;}
	//~ if (coreID == 7 ){J[2-1] = 128;}
	//~ if (coreID == 8 ){J[3-1] = 128;}
	//~ if (coreID == 9 ){J[4-1] = 128;}
	//~ if (coreID == 10){J[5-1] = 128;}
	//~ 
	uint allcores = 0;
	for(i=7; i<7+M+1; ++i){
		allcores |= 1 << i;
	}
	
	if(coreID<=M){
		spin1_set_timer_tick(1000);
		spin1_callback_on(TIMER_TICK, send_mc_packet_rates, 4);
		
	}else if(coreID==M+1){
		// motor commands shall be forwarded to the EAST port (IO board is
		// attached to the EAST port of chip 0)
		spin1_set_mc_table_entry(coreID, 0x0, 0x0, allcores);
		spin1_set_mc_table_entry(0, MOTOR_CMD_KEY, MOTOR_CMD_KEY, EAST);
		// eDVS packets shall be forwarded to the first core (0x80), the 
		// multicast packet has the key 0x02... with the lower 4 bytes 
		// containing the x/y coordinate of the ball
		spin1_set_mc_table_entry(1, 0x02000000, 0xffff0000, allcores);
		theta = -200;
		phi = 200;
		spin1_set_timer_tick(1000);
		spin1_callback_on(TIMER_TICK, send_mc_packet_angles, 1);
	}

	spin1_callback_on(MC_PACKET_RECEIVED,process_mc_packet,2);

	spin1_start();
}
