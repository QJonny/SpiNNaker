// author: Jonny Quarta
// Date: March 19st 2014

#include "ComTemplate.h"

#define MC_PAYLOAD 15

void updateMaster(uint sim_time, uint none)
{
        io_printf (IO_STD, "Master: sending MC packet to slave nodes\n");

        uint res = spin1_send_mc_packet(0x2, MC_PAYLOAD, WITH_PAYLOAD);

        if(res == 0) {
                io_printf (IO_STD, "Master: error in sending MC packet to slave nodes\n");
        }
}


void mcRec(uint key, uint payload){
	if(key == 0x2) {
		io_printf (IO_STD, "Slave: MC packet received (%d). Sending reply\n", payload);
		spin1_send_mc_packet(0x3, MC_PAYLOAD << 2, WITH_PAYLOAD);
	}
	else if(key == 0x3) {
		float p = (float)(payload);
		io_printf (IO_STD, "Master: MC reply received (%d).\n", (int)p);
	}
}

/*void mcRecMaster(uint key, uint payload){
	 io_printf (IO_STD, "Slave: MC packet received from master node with key: %d, payload %d\n", key, payload);
}
*/

// The idea is to set a communication system whose purpose
// is to exchange data from a virtual master node (situated
// at core 0, chip 0) and all the other cores situated on all
// the other chips. From master node to other nodes we use multicast
// messages, whereas from a node to master node we use sdp messages.
void c_main (void)
{		
	// simulation initialization
	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();

	//spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);

	io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	spin1_set_timer_tick(TICK_TIME);
	// end of simulation initialization


	
	if(chipID == 0 && coreID == 1) { // Master node
		//spin1_callback_on(SDP_PACKET_RX, sdpRecMaster, 2);
	        spin1_callback_on(TIMER_TICK, updateMaster, 0);
                spin1_callback_on(MC_PACKET_RECEIVED, mcRec, 0);
                // fill multicast table
	        spin1_set_mc_table_entry(0x3, 	0x2,	0xFFFFFFFF, N_EAST);
		spin1_set_mc_table_entry(0x4, 	0x3,	0xFFFFFFFF, CORE(1));
	}
        else { // Slave node
	        //spin1_callback_on(TIMER_TICK, updateSlave, 1);
                spin1_callback_on(MC_PACKET_RECEIVED, mcRec, 0);
                // fill multicast table
	        spin1_set_mc_table_entry(0x3, 	0x2,	0xFFFFFFFF, CORE(1));
		spin1_set_mc_table_entry(0x4, 	0x3,	0xFFFFFFFF, S_WEST);
        }

	spin1_start();
}


