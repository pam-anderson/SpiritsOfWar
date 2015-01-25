#include <stdio.h>
#include "altera_up_avalon_rs232.h"
#include "alt_types.h"
#include <sys/alt_irq.h>

// SYSTEM NAMES
#define RS232_NAME                 "uart_0"

// IRQ ID
#define SERIAL_IRQ                 8
#define KEYBOARD_IRQ               7

// MEMORY LOCATIONS
#define SERIAL_DATA_LOC (alt_u8 *) 0x0
#define SERIAL_PAR_LOC (alt_u8 *)  0x4

alt_up_rs232_dev *serial_port;

void read_serial(void* context, alt_u32 id) {
	alt_up_rs232_read_data(serial_port, SERIAL_DATA_LOC, SERIAL_PAR_LOC);
	printf("data: %d\n", *SERIAL_DATA_LOC);
}

int main()
{
  printf("Hello!\n");
  void* context = NULL;

  alt_up_rs232_enable_read_interrupt(serial_port);
  alt_irq_register(SERIAL_IRQ, context, &read_serial);

  serial_port =  alt_up_rs232_open_dev(RS232_NAME);
  while(1){
	  // Loop until there is a successful read
  }


  return 0;
}
