
#define NUMBER_OF_CHIPS    4
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

#define NUMBER_OF_XCHIPS 2
#define NUMBER_OF_YCHIPS 2

uint core_map[NUMBER_OF_XCHIPS][NUMBER_OF_YCHIPS] =
{
  {0, 0},
  {0, 0xffff}
};
uint coreID;
uint chipID;


#define MOTOR_CMD_KEY		(1 << 31)
#define MAKE_PAYLOAD(X, Y) 	((X) << 16 | (uint16_t)(Y))
#define HAS_PAYLOAD			1
#define NO_PAYLOAD			0

uint theta;
uint phi;


void reset_leds() {      
        spin1_led_control(LED_OFF(0) | LED_OFF(1));
}

void send_angles() {
        io_printf(IO_STD, "sending motor coord - core %d\n", coreID);
 	//uint32_t payload = MAKE_PAYLOAD(theta, phi);
	
	//spin1_send_mc_packet(MOTOR_CMD_KEY, payload, HAS_PAYLOAD);
        spin1_send_mc_packet(2, 10, HAS_PAYLOAD);
}

void process_mc_packet(uint key, uint payload){
        if(key == coreID) {
                io_printf(IO_STD, "Core id: %d - packet received\n", coreID);
        } else {
                io_printf(IO_STD, "Core id: %d - was not a packet for me\n", coreID);
        }
}

int c_main() {
        theta = -200;
        phi = 200;

        spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);


	coreID = spin1_get_core_id();
	chipID = spin1_get_chip_id();
	spin1_delay_us(coreID*100);

        io_printf(IO_STD, "Core id: %d, Chip id: %d - started\n", coreID, chipID);


	spin1_set_timer_tick(1000000);
	spin1_callback_on(TIMER_TICK, send_angles, 1);
		
        uint allcores = 0xFFFF;
	//spin1_set_mc_table_entry(coreID, 0x0, 0x0, allcores);
	//spin1_set_mc_table_entry(0, MOTOR_CMD_KEY, MOTOR_CMD_KEY, EAST);

	//spin1_set_mc_table_entry(1, 0x02000000, 0xffff0000, allcores);


	spin1_callback_on(MC_PACKET_RECEIVED,process_mc_packet,2);


	spin1_start();
}
