// author: Marcello Mulas
// Date: December 1st 2013

#include "spin1_api.h"
#include "spinn_io.h"
#include "spinn_sdp.h"

#define PRINT_DLY          200
#define MS_TICKS          1000
#define SEC_TICKS          MS_TICKS * 1000



void c_main (void)
{
  int i,j;
  int chipIDs[] = {0, 1, 256, 257};
  io_printf (IO_STD, "Program starting...!\n");

  uint coreID = spin1_get_core_id();
  uint chipID = spin1_get_chip_id();


  io_printf (IO_STD, "Program stoping...!\n");

}


