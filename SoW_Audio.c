#include "SoW.h"


void audio_init()
{
		printf("Initializing Audio\n");
		alt_up_av_config_dev * audio_init = alt_up_av_config_open_dev("/dev/audio_and_video_config_0");
		if (!alt_up_av_config_read_ready(audio_init))
		{
			printf("Audio can not be initialised\n");
		}
}

void audio_play()
{


	 unsigned int buff[2];
	  buff[0] = 0x00007FFF;
	  buff[1] = 0x00008000;


	  alt_up_audio_dev * audio = alt_up_audio_open_dev("/dev/audio_0"); // Open Device

	  if(audio == NULL)
		  printf("Audio Device Could not Open!\n");
	  else
		  printf("Audio Device Opened\n");

	  alt_up_audio_reset_audio_core(audio); // Clear the Audio Cores


	  while(1)
	  {
		 // printf("Printing!!!");
		  alt_up_audio_write_fifo(audio, buff, 2, ALT_UP_AUDIO_LEFT);
		  alt_up_audio_write_fifo(audio, buff, 2, ALT_UP_AUDIO_RIGHT);
	  }
	  	  printf("Exiting While\n");
}



/*int main()
{
  audio_init();
  audio_play();

  return 0;
}*/


