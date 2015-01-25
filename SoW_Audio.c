#include "SoW.h"


void audio_init()
{
		alt_up_av_config_dev * audio_init = alt_up_av_config_open_dev("/dev/audiovideocfg");
		while(!alt_up_av_config_read_ready(audio_init)){}
}

void audio_play()
{


	 unsigned int buff[2];
	  buff[0] = 0x00007FFF;
	  buff[1] = 0x00008000;


	  alt_up_audio_dev * audio = alt_up_audio_open_dev("/dev/audio"); // Open Device

	  if(audio == NULL)
		  printf("Audio Device Could not Open!\n");
	  else
		  printf("Audio Device Opened\n");

	  alt_up_audio_reset_audio_core(audio); // Clear the Audio Cores


	  while(1)
	  {
		  alt_up_audio_write_fifo(audio, buff, 2, ALT_UP_AUDIO_LEFT);
		  alt_up_audio_write_fifo(audio, buff, 2, ALT_UP_AUDIO_RIGHT);
	  }
	  	  printf("Exiting While\n");
}



int main()
{
  audio_init();
  	  printf("Audio Initialization Successful\n");
  audio_play();

  return 0;
}


