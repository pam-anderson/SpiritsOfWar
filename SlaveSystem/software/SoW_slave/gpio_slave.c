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

#include <stdio.h>
#include <altera_up_avalon_ps2.h>
#include <altera_up_ps2_keyboard.h>
#include "io.h"

#define GPIO_BASE 0x4440

int write_gpio(char data) {
	IOWR_32DIRECT(GPIO_BASE, 8, data);
	IOWR_32DIRECT(GPIO_BASE, 4, 1); // Set ready flag
	while(IORD_32DIRECT(GPIO_BASE, 4) == 0) {} // Wait for done flag
	IOWR_32DIRECT(GPIO_BASE, 4, 0);
}



int main()
{
  printf("Hello from Nios II!\n");
  alt_up_ps2_dev  *  ps2;
  KB_CODE_TYPE  code_type ;
  unsigned  char  buff;
  char  ascii;
  char str[100];

  ps2  =  alt_up_ps2_open_dev("/dev/keyboard");
  alt_up_ps2_init(ps2);

  while(1)
   	 {
   		 if(decode_scancode(ps2, &code_type, &buff, &ascii) == 0)
   		 {
   			 if(code_type == KB_ASCII_MAKE_CODE)
   			 {
   				 translate_make_code(code_type, buff, str);
   				 printf("%c\n", str[0]);
   		  		 write_gpio(str[0]);
   			}
   		 }

   	 }

  return 0;
}
