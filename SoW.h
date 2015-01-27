#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include <altera_up_avalon_ps2.h>
#include <altera_up_ps2_keyboard.h>
#include "alt_types.h"
#include <sys/alt_irq.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "altera_up_avalon_rs232.h"
#include <stdio.h>
#include "io.h"
#include <stdlib.h>


/* IRQ ID */
#define SERIAL_IRQ 8
#define KEYBOARD_IRQ 7

/* SYSTEM NAMES*/
#define RS232_NAME "uart_0"

/* MEMORY LOCATIONS */
#define SERIAL_DATA_LOC (alt_u8 *) 0x0
#define SERIAL_PAR_LOC (alt_u8 *) 0x4

#define SERIAL_BASE (volatile int *) 0x4070
#define KEY_BASE 0x4078

char *KeyInput;



/* Functions defined by startup code */

void keyboard_init(void);
void keyboard_read(void);


void audio_init(void);
void audio_play(void);

void serial_init(void);
void serial_write(alt_u8);

void sdcard_init(void);
int sdcard_write_file(char*, int*, int);
int sdcard_read_file(char*, int*, int);




