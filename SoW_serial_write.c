/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include "SoW.h"

alt_up_rs2323_dev* serial;

void serial_init() {
	serial = alt_up_rs232_open_dev("/dev/rs232_0");
}

int serial_write(alt_u8 data) {
	if(alt_up_rs232_get_available_space_in_write_FIFO(serial) > 0) {
		return alt_up_rs232_write_data(serial, data);
	}
	else {
		return -1;
	}
}

/*int main(void) {

	alt_up_rs232_dev* serial = alt_up_rs232_open_dev("/dev/rs232_0");
	while(1) {
		if(alt_up_rs232_get_available_space_in_write_FIFO(serial) > 0) {
			printf("Space: %d, Data: %d\n", alt_up_rs232_get_available_space_in_write_FIFO(serial), alt_up_rs232_write_data(serial, 'a'));
		}

	}
	return 0;
}*/

