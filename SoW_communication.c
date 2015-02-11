#include "SoW.h"

alt_up_ps2_dev  *ps2;

void keyboard_init(void){
  	 ps2  =  alt_up_ps2_open_dev("/dev/keyboard");
  	 alt_up_ps2_init(ps2);
  	 printf("Device : %d\n", ps2->device_type);
 }

char keyboard_read(void) {
	char read[1];
	char ascii;
	unsigned char buff;
	KB_CODE_TYPE  code_type;

  	 while(1) {
  		 if(decode_scancode(ps2, &code_type, &buff, &ascii) == 0) {
  			 if(code_type == KB_ASCII_MAKE_CODE) {
  				 translate_make_code(code_type, buff, read);
  				 printf("%c\n", read[0]);
  		  		 return read[0];
  			}
  		 }
  	 }
 }

char gpio_read(void) {
	char read;
	while(IORD_32DIRECT(GPIO_BASE, 0) == 0) {} // Wait for ready flag
	read = IORD_32DIRECT(GPIO_BASE, 4);
	IOWR_32DIRECT(GPIO_BASE, 0, 1); // Set done flag
	while(IORD_32DIRECT(GPIO_BASE, 0) == 1) {}
	IOWR_32DIRECT(GPIO_BASE, 0, 0);
}

/*
 * @brief This function obtains the player's input. It uses the player id to determine the input method.
 * @param player_id Player id
 */
keypress get_player_input(int player_id) {
	char cmd;
	while(1) {
		if(player_id == 0) {
			// Read directly from keyboard
			cmd = keyboard_read();
		} else {
			// Read from GPIO
			cmd = gpio_read();
		}

		if ((cmd == 'w') || (cmd == 'W')) {
			return UP;
		} else if ((cmd == 's') || (cmd == 'S')) {
			return DOWN;
		} else if ((cmd == 'a') || (cmd == 'A')) {
			return LEFT;
		} else if ((cmd == 'd') || (cmd == 'D')) {
			return RIGHT;
		} else if (cmd == '/') {
			return ENTER;
		} else if (cmd == ']') {
			return ESC;
		} else if ((cmd == 'n') || (cmd == 'N')) {
			return NEXT;
		}
	}
}

