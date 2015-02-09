#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_up_avalon_audio.h"
#include <altera_up_avalon_ps2.h>
#include <altera_up_ps2_keyboard.h>
#include "alt_types.h"
#include <sys/alt_irq.h>
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "Altera_UP_SD_Card_Avalon_Interface.h"

/* Defines */
#define	FALSE 0
#define	TRUE 1

/* Universal return constants */
#define	OK 1 /* system call ok */
#define	SYSERR -1 /* system call failed */
#define	TIMEOUT -3 /* time out (usu. recvtim) */

/* IRQ ID */
#define SERIAL_IRQ 8
#define KEYBOARD_IRQ 7
#define AUDIO_IRQ	9

/* SYSTEM NAMES*/
#define RS232_NAME "/dev/rs232_0"

/* MEMORY LOCATIONS */
#define SERIAL_DATA_LOC (alt_u8 *) 0x0
#define SERIAL_PAR_LOC (alt_u8 *) 0x4
#define DRAWER_BASE (volatile int*) 0x4400
#define SERIAL_BASE (volatile int *) 0x4070
#define KEY_BASE 0x4078
#define TIMER_BASE 0x4000

char *KeyInput;

/* Functions defined by startup code */
void keyboard_init(void);
char* keyboard_read(void);
void keyboard_enable_ISR(void);

void audio_init(void);
void audio_play(void);

int serial_write(alt_u8);
void serial_read();
void serial_read_ISR(void* context, alt_u32 id);
void draw_menu(void);
void move_arrow(int curr_position, int new_position);
void show_instructions(void);
void show_menu(void);
void show_game(void);
void animate(int, int, int, int, int);
void animate_to_tile(int, int, int, int, int, int, int);
void init_timer(void);

void sdcard_init() ;
int sdcard_write_file(char* file_name, alt_u8* buffer, int size) ;
int sdcard_read_file(char* file_name, alt_u8* buffer, int size);

alt_up_rs232_dev *serial_port;
alt_up_char_buffer_dev *char_buffer;
alt_up_pixel_buffer_dma_dev* pixel_buffer;


void music_load(char * File);
void music_init(void);
void audio_init(void);
void music_open(char *file);
void music_file_size(void);
void music_enable_ISR(void);
void audio_isr(void * context, alt_u32 id);
void music_file_load(void);
void music_GO(char * File);

void play_game(void);

