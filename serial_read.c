#include <stdio.h>
#include <sys/alt_irq.h>
#include "altera_up_avalon_rs232.h"
#include "alt_types.h"
#include "SoW.h"

#define CTL_REG (volatile char*) 0x2094

void read_serial_ISR(void* context, alt_u32 id) {
	alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC);
	printf("data: %d\n", *SERIAL_DATA_LOC);
}

void read_serial()
{
  printf("Hello!\n");
  int err;

  serial_port = alt_up_rs232_open_dev(RS232_NAME);
//  alt_irq_enable(SERIAL_IRQ);
//  err = alt_irq_register(SERIAL_IRQ, 0, (void*) read_serial_ISR);
//  alt_up_rs232_enable_read_interrupt(serial_port);
 /* if(err != 0) {
	  printf("Error registering interrupt: %d\n", err);
	  return 0;
  }
  if(alt_irq_enabled() == 0) {
	  printf("Interrupt not enabled \n");
	  return 0;
  }*/

 /* while(1){
	  // Wait for interrupt
	//  printf("Words to read: %d\n", alt_up_rs232_get_used_space_in_read_FIFO(serial_port));
	//  printf("Control reg: %x\n", *CTL_REG);
  }*/

  return;
}
