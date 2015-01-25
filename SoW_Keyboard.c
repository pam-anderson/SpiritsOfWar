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
#include "alt_types.h"
#include <sys/alt_irq.h>
#include <altera_avalon_uart_regs.h>

#define KEYBOARD_IRQ 7
#define KEY_BASE 0x2080
#define SERIAL_PAR_LOC (alt_u8 *) 0x8

  KB_CODE_TYPE  code_type ;
  unsigned  char  buff;
  char  ascii;
  alt_up_ps2_dev  *  ps2;
  char *inputStr;

  void keyboard_init(void)
  {
  	int i ;
  	  ps2  =  alt_up_ps2_open_dev("/dev/keyboard");
  	  alt_up_ps2_init(ps2);
  	  for(i=0; i<10000;i++);
  	  printf("Device : %d\n", ps2->device_type);
  }

  void keyboard_read(void * context)
  {
  	 while(1)
  	 {
  		 if(decode_scancode(ps2, &code_type, &buff, &ascii) == 0)
  		 {
  			 if(code_type == KB_ASCII_MAKE_CODE)
  			 {
  				 translate_make_code(code_type, buff, inputStr);
  		  		 printf("%c", *inputStr);
  			}
  		 }

  	 }

  }



/*int main()
{
	keyboard_init();
	//void * context = NULL;
	alt_irq_enable(ps2);
	IOWR(KEY_BASE, 1, 0x01);
	alt_irq_register(ps2, context, &keyboard_read);
	while(1);
	printf("Returning\n");

	// Configure keyboard to send interrupt



  while(1)
  {
	  decode_scancode(ps2,  &code_type,  &buff,  &ascii);
	  if(code_type != 6)
		  printf("Decode mode: %d Buffer : 0x%X ASCII: %c\n", code_type, buff, ascii);
  }

  return 0;
}*/

