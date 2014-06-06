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


#define LOG_P1                  (8)
#define LOG_P2                  (16)
#define P1						(1 << LOG_P1)
#define P2						(1 << LOG_P2)

#define M 	15

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


uint x_old_[20];
uint y_old_[20];
uint x_ = 0;
uint y_ = 0;
uint count = 0;


long F = 10 << LOG_P1;
//~ int F = 100;

long Iext;

long J[M];
long Fpre[M];
long U[M];
long x[M];
long Use[M];
long Tr[M];
long Tf[M];

long abs_(long n){
return n < 0 ? -n : n;
}


void send_mc_packet_angles() {	
	//~ theta = 10;
	//~ phi -= 10;
	
	theta = (int)((Fpre[11-1] - Fpre[ 9-1]  + (11 << LOG_P1))*8) >> LOG_P1;
	phi   = (int)((Fpre[10-1] - Fpre[12-1]  + (3 << LOG_P1))*8) >> LOG_P1;
	//~ theta = (int)(0) >> LOG_P1;
	//~ phi   = (int)(0) >> LOG_P1;

	count += 1;
	// io_printf(IO_STD,"count is %d \n", count);

	if (count == 30000){
		theta = 0;
		phi = 300;
		uint32_t payload = MAKE_PAYLOAD(theta, phi);
		spin1_send_mc_packet(MOTOR_CMD_KEY, payload, HAS_PAYLOAD);
		spin1_delay_us(4000000);
		count=0;
	}
    
    
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



long IFfct(long long input){
	
	long long output = 0;
	
	long long thr1 = 50 << LOG_P1;
	//~ long long scf = 2 * 256;
	//~ long long scf = 2*256;
	long long scf = 400;
	
	if(input < thr1){output = 0;}
	else{output = ((input - thr1) * scf ) >> LOG_P1;}
	
	if(output > (100 << LOG_P1)){output = 100 << LOG_P1;}
	
	return output;
	
}



void send_mc_packet_rates() {
	
	

	//~ io_printf(IO_STD,"----------------------\n");

	//~ long v_decay = 64 << LOG_P1;
	//~ long v_decay = 32 << LOG_P1;
	long v_decay = 40 << LOG_P1;
	//~ long v_decay = 2 << LOG_P1;
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
		//~ summ += ((J[i]*(256-(x[i]>>LOG_P1))*Fpre[i]) >> LOG_P2) ;
		//~ summ += ((J[i]*Fpre[i]) >> LOG_P1) * ((256 - x[i])>>LOG_P1);
		summ += (((J[i]*Fpre[i]) >> LOG_P1) );
		//~ summ += (((J[i]*Fpre[i]) >> LOG_P1) * ((256 - x[i])))>>LOG_P1;
		//~ long dx = (( - x[i]) * x_decay) >> LOG_P1;
		//~ int Add = 1*(10*(F>>LOG_P1))>>LOG_P1;
		//~ x[i] = x[i] + dx + Add;

		//~ x[i] = 0;
		//~ if(i==4-1 && coreID==9){ if(F>0){spin1_delay_us(coreID*100);io_printf(IO_STD,"x: %d, F: %d, J*Fpre: %d, Add: %d \n",(x[i]), F, IFfct( (J[i]*Fpre[i]) >> LOG_P1), Add); } }
	}
	long dv = (( - F + IFfct( summ + Iext ) ) * v_decay ) >> LOG_P1 ;
	dv = dv >> LOG_P1;
	
	
	//~ io_printf(IO_STD,"Dv %d\n", dv >> LOG_P1 );
	
	F = F + dv;

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
        
        uint iivv = 0;
        for(; iivv<20-1; iivv+=1){
            x_old_[iivv] = x_old_[iivv+1];    
            y_old_[iivv] = y_old_[iivv+1];    
        }
		x_old_[20-1] = x_;
		y_old_[20-1] = y_;
		x_ = GET_X_COORD(key);
		y_ = GET_Y_COORD(key);
		
        int32_t Vx_ = ((int32_t)x_) - ((int32_t)x_old_[0]);
        int32_t Vy_ = ((int32_t)y_) - ((int32_t)y_old_[0]);
        
		
		if(coreID==M+1){
			//~ int32_t t = ((int32_t)x - 64) * 6;
			//~ int32_t p = ((int32_t)y - 64) * 6;
			
			//~ theta = t;
			//~ phi = -p; // inverse sign for this direction

			//~ io_printf(IO_STD,"X: %3u, Y: %3u \n", ( (((int32_t)x_-64)*((int32_t)x_-64) + ((int32_t)y_-64)*((int32_t)y_-64))) >> LOG_P1, 1000+Vy_);
			//~ io_printf(IO_STD,"X: %3u, Y: %3u \n", x_, y_);

		}
				
		//~ Iext = 50 << LOG_P1;
		Iext = 50 << LOG_P1;
		//~ if((int32_t)x_<64 && coreID==1){Iext = -1000 * ((int32_t)x_-64);}
		//~ if((int32_t)x_>64 && coreID==3){Iext =  1000 * ((int32_t)x_-64);}
		//~ if((int32_t)y_<64 && coreID==2){Iext = -1000 * ((int32_t)y_-64);}
		//~ if((int32_t)y_>64 && coreID==4){Iext =  1000 * ((int32_t)y_-64);}
        
        if(((int32_t)x_-60)*((int32_t)x_-60) + ((int32_t)y_-64)*((int32_t)y_-64)<390){
            if((int32_t)x_<64 && coreID==1){Iext =  54 << LOG_P1;}
            if((int32_t)x_>64 && coreID==3){Iext =  54 << LOG_P1;}
            if((int32_t)y_<64 && coreID==2){Iext =  54 << LOG_P1;}
            if((int32_t)y_>64 && coreID==4){Iext =  54 << LOG_P1;}

            if((int32_t)Vx_<0 && coreID==5){Iext += (-Vx_ *0) << LOG_P1;}
            if((int32_t)Vx_>0 && coreID==7){Iext += ( Vx_  *0)<< LOG_P1;}
            if((int32_t)Vy_<0 && coreID==6){Iext += (-Vy_ *0) << LOG_P1;}
            if((int32_t)Vy_>0 && coreID==8){Iext += ( Vy_  *0)  << LOG_P1;}

		}else{
            if((int32_t)x_<64 && coreID==1){Iext =  58 << LOG_P1;}
            if((int32_t)x_>64 && coreID==3){Iext =  58 << LOG_P1;}
            if((int32_t)y_<64 && coreID==2){Iext =  58 << LOG_P1;}
            if((int32_t)y_>64 && coreID==4){Iext =  58 << LOG_P1;}

            if((int32_t)Vx_<0 && coreID==5){Iext += (-Vx_ *4) << LOG_P1;}
            if((int32_t)Vx_>0 && coreID==7){Iext += ( Vx_  *4)<< LOG_P1;}
            if((int32_t)Vy_<0 && coreID==6){Iext += (-Vy_ *4) << LOG_P1;}
            if((int32_t)Vy_>0 && coreID==8){Iext += ( Vy_  *4)  << LOG_P1;}

        }
        
		//~ if((int32_t)Vx_<0 && coreID==5){Iext += (-Vx_ *4) << LOG_P1;}
		//~ if((int32_t)Vx_>0 && coreID==7){Iext += (Vx_  *4)<< LOG_P1;}
		//~ if((int32_t)Vy_<0 && coreID==6){Iext += (-Vy_ *4) << LOG_P1;}
		//~ if((int32_t)Vy_>0 && coreID==8){Iext += (Vy_  *4)  << LOG_P1;}
		
        //~ if(((int32_t)x_-64)*((int32_t)x_-64) + ((int32_t)y_-64)*((int32_t)y_-64)<150 && coreID==13){Iext += 20  << LOG_P1;}
        //~ if(coreID==13){Iext += ( 2*128*128 - (((int32_t)x_-64)*((int32_t)x_-64) + ((int32_t)y_-64)*((int32_t)y_-64))) >> LOG_P1 ;}
		
        
		
	}
	else if(key<=M){
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
	// io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	//~ spin1_set_core_map(NUMBER_OF_CHIPS, core_map);
	
	//~ spin1_dselay_us(100000);
	
	for(i=0; i<M; ++i){
		Fpre[i] = 0;
		x[ i] = P1;
		U[ i] = 0;
		J[ i] = 0;
		Tr[i] = 64;
		Tf[i] = 64;
		Use[i]= 25; // 256 * 0.1 ?
	}
	
    
	if (coreID == 9 ){J[1-1] = 300;}
	if (coreID == 10 ){J[2-1] = 300;}
	if (coreID == 11 ){J[3-1] = 300;}
	if (coreID == 12){J[4-1] = 300;}

	if (coreID == 9 ){J[5-1] = 300;}
	if (coreID == 10 ){J[6-1] = 300;}
	if (coreID == 11 ){J[7-1] = 300;}
	if (coreID == 12){J[8-1] = 300;}

	//~ if (coreID == 9 ){J[13-1] = 200;}
	//~ if (coreID == 10 ){J[13-1] = 200;}
	//~ if (coreID == 11 ){J[13-1] = 200;}
	//~ if (coreID == 12){J[13-1] = 200;}



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
		// spin1_set_mc_table_entry(2, 0x45 | 0x400, 0xFFFFFFFF, EAST);
		// eDVS packets shall be forwarded to the first core (0x80), the 
		// multicast packet has the key 0x02... with the lower 4 bytes 
		// containing the x/y coordinate of the ball
		spin1_set_mc_table_entry(1, 0x02000000, 0xffff0000, allcores);
		theta = -200;
		phi = 200;
		spin1_set_timer_tick(1000);
		spin1_callback_on(TIMER_TICK, send_mc_packet_angles, 1);

		// spin1_send_mc_packet(0x45 | 0x400, 1,1);
	}

	spin1_callback_on(MC_PACKET_RECEIVED,process_mc_packet,2);

	spin1_start();
}
