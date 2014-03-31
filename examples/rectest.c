
#define NUMBER_OF_CHIPS    4
//~ #define CORE_MAP_00        0x0000e
#define CORE_MAP_00        0x00002
#define CORE_MAP_01        0x00000
#define CORE_MAP_10        0x00000
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

uint core_map[NUMBER_OF_CHIPS];
uint coreID;
uint chipID;

int theta, phi;

#define MOTOR_CMD_KEY		(1 << 31)
#define MAKE_PAYLOAD(X, Y) 	((X) << 16 | (uint16_t)(Y))
#define HAS_PAYLOAD			1
#define NO_PAYLOAD			0

// #define EXTRACT_X_PAYLOAD(P)

#define GET_X_COORD(KEY)	(((KEY) & 0xFF00) >> 8)
#define GET_Y_COORD(KEY)	((KEY) & 0xFF)

void send_mc_packet() {	
	//~ theta = 10;
	//~ phi -= 10;
	
	uint32_t payload = MAKE_PAYLOAD(theta, phi);
	
	spin1_send_mc_packet(MOTOR_CMD_KEY, payload, HAS_PAYLOAD);
	// io_printf(IO_STD,"sending packet ... %x \n", payload);
}

void process_mc_packet(uint key, uint payload){
	uint x = GET_X_COORD(key);
	uint y = GET_Y_COORD(key);
	
	int32_t t = ((int32_t)x - 64) * 6;
	int32_t p = ((int32_t)y - 64) * 6;
	
	theta = t;
	phi = -p; // inverse sign for this direction
	
	io_printf(IO_STD,"X: %3u, Y: %3u, T: %4d, P: %4d\n", x, y, t, p);
			
}

int c_main() {
	
	if (NUMBER_OF_CHIPS > 0) core_map[0] = CORE_MAP_00;
	if (NUMBER_OF_CHIPS > 1) core_map[1] = CORE_MAP_01;
	if (NUMBER_OF_CHIPS > 2) core_map[2] = CORE_MAP_10;
	if (NUMBER_OF_CHIPS > 3) core_map[3] = CORE_MAP_11;

	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();
	
	io_printf(IO_STD,"CoreID is %u, ChipID is %u\n",coreID, chipID);

	//~ spin1_set_core_map(NUMBER_OF_CHIPS, core_map);
	
	//~ spin1_delay_us(100000);

	// motor commands shall be forwarded to the EAST port (IO board is
	// attached to the EAST port of chip 0)
	spin1_set_mc_table_entry(0, MOTOR_CMD_KEY, MOTOR_CMD_KEY, EAST);
	
	// eDVS packets shall be forwarded to the first core (0x80), the 
	// multicast packet has the key 0x02... with the lower 4 bytes 
	// containing the x/y coordinate of the ball
	spin1_set_mc_table_entry(1, 0x02000000, 0xffff0000, 0x80);

	
	theta = -400;
	phi = 400;
	spin1_set_timer_tick(100000);
	spin1_callback_on(TIMER_TICK, send_mc_packet, 1);
	
	
	spin1_callback_on(MC_PACKET_RECEIVED,process_mc_packet,2);

	spin1_start();
}
