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

void updateSlave(uint sim_time, uint none)
{
	sdp_msg_t* msg = spin1_msg_get();
	//msg->length = 36; should be explicitely declared?

	msg->flags = 0x07;
	msg->tag = 0;
	msg->srce_port = (0x1<<5) | coreID; // core number, port 1
	msg->dest_port = (0x1<<5); // core 0, port 1
	msg->srce_addr = chipID; // current chip identifier
	msg->dest_addr = 0x0; // on chip 0

	msg->data[0] = chipID;
	msg->data[1] = coreID;

        io_printf (IO_STD, "Slave: sending SDP packet to master node\n");
	spin1_send_sdp_msg(msg, 100);

	spin1_msg_free(msg);
}


void sdpRecMaster(uint m, uint port){
	sdp_msg_t* msg = (sdp_msg_t*)m;

	io_printf (IO_STD, "Master: SDP packet received from (%d, %d)\n", (uint)msg->data[0], (uint)msg->data[1]);

	spin1_msg_free(msg);
}

void mcRecSlave(uint key, uint payload){
	 io_printf (IO_STD, "Slave: MC packet received from master node with key: %d, payload %d\n", key, payload);
}



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
	        spin1_callback_on(TIMER_TICK, updateMaster, 1);
                // fill multicast table
	        spin1_set_mc_table_entry(0x3, 	0x2,	0xffff, CORE(2)|CORE(3)|N_EAST);
	}
        else { // Slave node
	        //spin1_callback_on(TIMER_TICK, updateSlave, 1);
                spin1_callback_on(MC_PACKET_RECEIVED, mcRecSlave, 2);
                // fill multicast table
		spin1_set_mc_table_entry(0x3, 	0x2,	0xffff, CORE(2)|CORE(3));
	        //spin1_set_mc_table_entry(0x3, 	0x0,	0x0, CORE(1));
        }

	spin1_start();
}


