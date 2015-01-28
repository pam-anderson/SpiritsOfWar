#include "alt_types.h"
#include "altera_up_avalon_rs232.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"

#define	FALSE   0     
#define	TRUE    1

/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */

/* IRQ ID */
#define SERIAL_IRQ   8
#define KEYBOARD_IRQ 7

/* SYSTEM NAMES*/
#define RS232_NAME "/dev/rs232_0"

/* MEMORY LOCATIONS */
#define SERIAL_DATA_LOC (volatile alt_u8 *) 0x0
#define SERIAL_PAR_LOC (volatile alt_u8 *)  0x4
#define SERIAL_BASE (volatile int *)        0x4470

#define SIZE_OF_TILE     16
#define SIZE_OF_MAP      128
#define DIMENSION_OF_MAP 8

typedef struct {
	// x and y coordinates of top left corner
	int x;
	int y;
} game_tile;

/* Functions defined by startup code */

void keyboard_init(void);
void keyboard_read(void * context);

// void serial_init(void); 
void read_serial();
void read_serial_ISR(void* context, alt_u32 id);
void draw_menu(void);
void move_arrow(int curr_position, int new_position);
void show_instructions(void);
void show_menu(void);
void show_game(void);

alt_up_rs232_dev *serial_port;
alt_up_char_buffer_dev *char_buffer;
alt_up_pixel_buffer_dma_dev* pixel_buffer;
