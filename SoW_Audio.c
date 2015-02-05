#include "SoW.h"


char* FileSelect;
short int fd = 0;
int WavSize = 0;
unsigned int *MusicBuff;
unsigned int *MusicData;
int MusicDataIndex = 0;
int MusicDataCount;
alt_up_audio_dev *audio;


void music_GO(char * File)
{
	music_load(File);
	audio_init();
	music_enable_ISR();
}

void music_load(char * File)
{
	music_init();

	music_open(File);

	music_file_size();

	music_file_load();	

}

void music_init(void)
{
	alt_up_av_config_dev * audio_init = alt_up_av_config_open_dev("/dev/audio_and_video_config_0");

	if(audio_init != NULL)
		printf("Audio Config Set Up!\n");


	alt_up_sd_card_dev *device = NULL;
	device = alt_up_sd_card_open_dev("/dev/sdcard");

	if(device != NULL)
	{
		printf("Device is not Null!\n");
		if(alt_up_sd_card_is_Present())
		{
			printf("SD Card Present!\n");
			if(alt_up_sd_card_is_FAT16())
				printf("SD Card is FAT16!\n");
		}
	}

	printf("Music_init done!\n");
}

void audio_init(void)
{
	int i ; 
	
	audio = alt_up_audio_open_dev("/dev/audio_0");
	
	alt_up_audio_reset_audio_core(audio);
	
	MusicBuff = (unsigned int *) malloc (110 * sizeof(unsigned int)); 
	
	for(i = 0; i < 110; i++)
	{
		MusicBuff[i] = MusicData[MusicDataIndex];
		
		if(MusicDataIndex >=  MusicDataCount)
			MusicDataIndex = 0; 
		else 
			MusicDataIndex++; 
	}
}

void music_open(char *file)
{
		fd = alt_up_sd_card_fopen(file, 0);
		if(fd == -1)
			printf("Error Opening File\n");
		else if (fd == -2)
			printf("File Already Open\n");
		else
			printf("File is now Opened!\n");
}

void music_file_size(void)
{
	unsigned int header[40];
	unsigned int size[4];
	int temp, i, j = 0; 
	
	/* Cycle Through Header */ 
	for(i = 0; i < 40; i++)
		header[i] = alt_up_sd_card_read(fd);
		
	for(i = 4; i < 8; i++)
	{
		size[j] = header[i];
		j++;
	}

	/* Reverse Array */
	for(i = 0; i < 2; ++i)
	{
		temp = size[i];
		size[i] = size[4-i-1];
		size[4-i-1] = temp;
	}
	
	WavSize = (size[0] << 24 | size[1] << 16 | size[2] << 8 | size[3]) + 8;

}

void music_enable_ISR(void)
{
	if(alt_irq_register(AUDIO_IRQ, NULL, audio_isr) == 0)
		printf("IRQ Registered\n");
	else
		printf("IRQ Not Registered\n");
	alt_up_audio_enable_write_interrupt(audio);
}

void audio_isr(void * context, alt_u32 id)
{
	int BuffCount; 
	for(BuffCount = 0; BuffCount < 110; BuffCount++)
	{
		MusicBuff[BuffCount] = MusicData[MusicDataIndex];

		if(MusicDataIndex >= MusicDataCount)
			MusicDataIndex = 0;
		else 
			MusicDataIndex++;
	}
	
	alt_up_audio_write_fifo(audio, MusicBuff, 110, ALT_UP_AUDIO_LEFT); 
	alt_up_audio_write_fifo(audio, MusicBuff, 110, ALT_UP_AUDIO_RIGHT);
}

void music_file_load(void)
{

	int i,j; 
	short k;
	unsigned int audio_data[2];
	MusicDataCount = 0;
	
	/* 110 is sample size */
	MusicBuff = (unsigned int *) malloc (110 * sizeof(unsigned int)); 
	MusicData = (unsigned int *) malloc (WavSize/2 * sizeof(unsigned int)); 
	
	for(i = 0; i < WavSize/2 ; i++)
	{
		for(j = 0; j < 2; j++)
			audio_data[j] = alt_up_sd_card_read(fd) & (0x00FF); 
		
		k = (unsigned char) audio_data[1] << 8 | (unsigned char) audio_data[0]; 
		
		MusicData[MusicDataCount] = k;
		
		MusicDataCount++;

	}
}

	
