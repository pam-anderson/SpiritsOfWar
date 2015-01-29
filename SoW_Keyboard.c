#include "SoW.h"




  KB_CODE_TYPE  code_type ;
  unsigned  char  buff;
  char  ascii;
  alt_up_ps2_dev  *  ps2;


  void keyboard_init(void)
  {
  	int i ;
  	  ps2  =  alt_up_ps2_open_dev("/dev/keyboard");
  	  alt_up_ps2_init(ps2);
  	  for(i=0; i<10000;i++);
  	  printf("Device : %d\n", ps2->device_type);
  }

  void keyboard_read(void)
  {
	 char c;
  	 while(1)
  	 {
  		 if(decode_scancode(ps2, &code_type, &buff, &ascii) == 0)
  		 {
  			 if(code_type == KB_ASCII_MAKE_CODE)
  			 {
  				 translate_make_code(code_type, buff, KeyInput);
  				 printf("%c\n", *KeyInput);

  		  		 return;
  			}
  		 }

  	 }

  }

  void keyboard_ISR(void * context, alt_u32 id)
  {
	 char c;
  	 while(1)
  	 {
  		 if(decode_scancode(ps2, &code_type, &buff, &ascii) == 0)
  		 {
  			 if(code_type == KB_ASCII_MAKE_CODE)
  			 {
  				 translate_make_code(code_type, buff, KeyInput);
  				 printf("%c\n", *KeyInput);

  		  		 return;
  			}
  		 }

  	 }

  }

  void keyboard_enable_ISR(void)
  {
  	if(alt_irq_register(KEYBOARD_IRQ, NULL, keyboard_ISR) == 0)
  		printf("IRQ Registered\n");
  	else
  		printf("IRQ Not Registered\n");
  	alt_up_audio_enable_write_interrupt(ps2);
  }





/*int main()
{
	keyboard_init();
	keyboard_read();
	keyboard_read();
	keyboard_read();
	keyboard_read();
}*/
	//void * context = NULL;
	/*alt_irq_enable(ps2);
	IOWR(KEY_BASE, 1, 0x01);
	alt_irq_register(ps2, &keyboard_read);
	while(1);
	printf("Returning\n");*/

	// Configure keyboard to send interrupt
/*


  while(1)
  {
	  decode_scancode(ps2,  &code_type,  &buff,  &ascii);
	  if(code_type != 6)
		  printf("Decode mode: %d Buffer : 0x%X ASCII: %c\n", code_type, buff, ascii);
  }

  return 0;
}*/

 /* void keyboard_enable_ISR(void)
  {
  	if(alt_irq_register(KEYBOARD_IRQ, NULL, keyboard_read) == 0)
  		printf("IRQ Registered\n");
  	else
  		printf("IRQ Not Registered\n");
  	alt_up_audio_enable_write_interrupt(ps2);
  }*/

