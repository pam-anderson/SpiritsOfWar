#define	FALSE   0     
#define	TRUE    1
#define	NULL    0       


/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */

/* IRQ ID */
#define SERIAL_IRQ 8
#define KEYBOARD_IRQ 7

/* SYSTEM NAMES*/
#define RS232_NAME "uart_0"

/* MEMORY LOCATIONS */
#define SERIAL_DATA_LOC (alt_u8 *) 0x0
#define SERIAL_PAR_LOC (alt_u8 *) 0x4

#define SERIAL_BASE (volatile int *) 0x4470;



/* Functions defined by startup code */

void keyboard_init(void);
void keyboard_read(void * context);

// void serial_init(void); 
void read_serial(void * context, alt_u32 id); 