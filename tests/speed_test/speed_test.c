// author: Jonny Quarta
// Date: March 19st 2014


#include "spin1_api.h"
#include "spinn_io.h"


// simulation constants
#define MS 1000
#define TICK_TIME (100*MS) // in ms
// end of simulation constants


typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

uint coreID;
uint chipID;

#define EAST 0x1
#define N_EAST 0x2
#define NORTH 0x4
#define WEST 0x8
#define S_WEST 0x10
#define SOUTH 0x20


#define CORE(n) (1<< (n+6))

#define ALL_NODES_MASTER_CHIP CORE(2)|CORE(3)|CORE(4)|CORE(5)|CORE(6)|CORE(7)|CORE(8)|CORE(9)|CORE(10)|CORE(11)|CORE(12)|CORE(13)|CORE(14)|CORE(15)
#define ALL_NODES_NO_MASTER_CHIP CORE(1)|ALL_NODES_MASTER_CHIP
#define MASTER_CORE CORE(1)

#define ERROR_MSG (uint)(0x20)
#define UPD_MSG (uint)(0x30)

uint nb_packets = 0;

// chip 0
void update_chip0(uint sim_time, uint none)
{
	io_printf(IO_STD,"sending error\n");
	uint key = ERROR_MSG;

	spin1_send_mc_packet(key, 0, WITH_PAYLOAD);
}





void eventNode_chip0(uint key, uint payload){
	if((key) == ERROR_MSG) {
		//io_printf(IO_STD, "rec err %u\n", spin1_get_simulation_time());
		uint key = UPD_MSG;
		spin1_send_mc_packet(key, 0, WITH_PAYLOAD);
	}
}



void eventMaster_chip0(uint key, uint payload){
	if((key) == UPD_MSG) {
		nb_packets++;

		if(nb_packets == 15) {
			io_printf(IO_STD, "rec upd %u\n", spin1_get_simulation_time());
			nb_packets = 0;
		}
	}
}





// chip 1
void update_chip1(uint sim_time, uint none)
{
	int i = 0;
	io_printf(IO_STD,"sending 4 msg\n");

	for(i = 0; i < 64; i++) {
		spin1_send_mc_packet(coreID + 1, 0, WITH_PAYLOAD);
	}
}




void eventRec_chip1(uint key, uint payload){
	nb_packets++;
	if(nb_packets % 64 == 0) {
		//io_printf(IO_STD, "rec msg %u\n", spin1_get_simulation_time());
		io_printf(IO_STD, "64 packets received, %u\n", spin1_get_simulation_time());
	}
}














void c_main (void)
{		
	// simulation initialization
	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();
	
	spin1_set_timer_tick(TICK_TIME);

	// the intrachip communication is totally independent from a
	// chip to another. chips do not interfere with each other (good point)


	if(chipID == 0) { // 0, 0
		// in this chip, we test master to nodes, then nodes to master
		// results: too slow!! nodes miss a lot of packets
		// master does receive only a small fraction of the responses
		// and this is only on a single chip with only 15 nodes (the total should be 64)!!!
		// without camera events!!! WHITHOUT ANY OTHER COMPUTATION!!!!!!


		// from master to nodes
		spin1_set_mc_table_entry(0x6, 	ERROR_MSG,	 0xFFFFFFFF, ALL_NODES_MASTER_CHIP);
		// froms nodes to master
		spin1_set_mc_table_entry(0x7, 	UPD_MSG,	 0xFFFFFFFF, MASTER_CORE);

		if(coreID == 1) { // master core
			// events setting
			spin1_callback_on(MC_PACKET_RECEIVED,eventMaster_chip0,0);
			spin1_callback_on(TIMER_TICK, update_chip0, 0);

		}
		else { // nodes
			// events setting
			spin1_callback_on(MC_PACKET_RECEIVED,eventNode_chip0,0);
		}


	}	
	else if(chipID == 1) { // 0, 1
		// in this chip, we test parallel message sending (core 1 to 2, core 3 to 4 and core 5 to 6, independently)
		// results: we can see that internode communication is not independent.
		// if we decomment the code in the condition, nodes will no more receive
		// all the incoming messages, which are only 4 per update!!!
		// this is because there is a bottleneck when reading the mc table.
		// VERY POOR PARALLELIZATION STRATEGY!! nodes communication should be
		// completely independent, if the purpose is to mimic the brain structure


		spin1_set_mc_table_entry(0x2, 	0x2,	 0xFFFFFFFF, CORE(2));
		spin1_set_mc_table_entry(0x4, 	0x4,	 0xFFFFFFFF, CORE(4));
		spin1_set_mc_table_entry(0x6, 	0x6,	 0xFFFFFFFF, CORE(6));


		if(coreID == 3 | coreID == 1 | coreID == 5) { // sending cores
			spin1_callback_on(TIMER_TICK, update_chip1, -2);

		}
		else { // receiving cores
			spin1_callback_on(MC_PACKET_RECEIVED,eventRec_chip1,-1);
		}

	}
	else if(chipID == 256) { // 1, 0
		// ...
	}
	else if(chipID == 257) { // 1, 1
		// ...
	}




	spin1_start();
}


