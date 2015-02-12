#include "SoW.h"

#define BYTE 8

int trigger = -1;

char* file_select;
short int fd = 0;

int wav_size = 0;
int event_size = 0;

unsigned int *music_buff;
unsigned int *music_data;

int music_data_index = 0;
int music_data_count;

static alt_up_audio_dev *audio;




/*
 *@brief Music Choose chooses which event sound to play.
 *@param a The array value for the desired audio event.
 */
void music_choose(int a)
{
	trigger =  a;
}

/*
 *@brief Music Go initializes the audio and loads the appropriate .wav files and starts the ISR
 */
void music_go(){
	music_init();
	audio_init();
	music_load("MUS.WAV");
	music_load("war.wav");
	music_load("arch.wav");
	music_load("mag.wav");
	music_load("mov.wav");
	music_load("die.wav");
	music_enable_ISR();
}

/*
 *@brief Muisc Load opens the music file, Reads the Meta-Data and Loads the music to RAM
 *@param File The filename of the file to be loaded.
 */
void music_load(char * file){
	file_select = NULL;
	file_select = malloc(sizeof(file));
	strcpy(file_select, file);
	music_open(file);
	music_file_size();
	music_file_load();
}

/*
 *@brief Music Init, loads the audio_video driver, opens the SD card and initializes the music event struct.
 */
void music_init(void){
	int i;
	alt_up_av_config_dev * audio_init = alt_up_av_config_open_dev("/dev/audio_and_video_config_0");
	if(audio_init != NULL){
		printf("Audio Config Set Up!\n");
		fflush(stdout);
	}
	else{
		printf("Audio Config Not Setup!\n");
		fflush(stdout);
	}
	alt_up_sd_card_dev *device = NULL;
	device = alt_up_sd_card_open_dev("/dev/sdcard");
	if(device == NULL){
		printf("SD Card Did not Open!\n");
		fflush(stdout);
	}
	else{
		printf("SD Card Opened!\n");
		fflush(stdout);
	}
	if(device != NULL)	{
		printf("Device is not Null!\n");
		fflush(stdout);
		if(alt_up_sd_card_is_Present())	{
			printf("SD Card Present!\n");
			fflush(stdout);
			if(alt_up_sd_card_is_FAT16())	{
				printf("SD Card is FAT16!\n");
				fflush(stdout);
			}
		}
	}
	for(i = 0; i < NumEvents; i++)	{
		Events[i].filename = NULL;
		Events[i].initialized = 0;
		Events[i].size = 0;
	}
	printf("Music_init done!\n");
	fflush(stdout);
}

/*
 * @brief Audio Init initializes the audio device
 */
void audio_init(void)
{
	int i ;
	audio = alt_up_audio_open_dev("/dev/audio_0");
	if(audio == NULL)	{
		printf("Audio Device Not Opened!\n");
		fflush(stdout);
		return;
	}
	else	{
		printf("Audio Device Opened!\n");
		fflush(stdout);
	}
	alt_up_audio_reset_audio_core(audio);
	music_buff = (unsigned int *) malloc (110 * sizeof(unsigned int));
	for(i = 0; i < 110; i++)	{
		music_buff[i] = music_data[music_data_index];

		if(music_data_index >=  music_data_count)
			music_data_index = 0;
		else
			music_data_index++;
	}
}

/*
 *@brief Music Open opens the file with the desired filename
 *@param file The filename of the file to be opened
 */
void music_open(char *file) {
		fd = alt_up_sd_card_fopen(file, 0);
		if(fd == -1){
			printf("Error Opening File\n");
			fflush(stdout);
		}
		else if (fd == -2){
			printf("File Already Open\n");
			fflush(stdout);
		}
		else{
			printf("File is now Opened!\n");
			fflush(stdout);
		}
}

/*
 *@brief Music File Size cycles through the header and finds the size of the music file to be loaded
 *@brief From the read in file it shifts the byte positions in order to position it in FIFO.
 */
void music_file_size(void){
	unsigned int header[40];
	unsigned int size[4];
	int temp, i, j = 0;
	for(i = 0; i < 40; i++)
		header[i] = alt_up_sd_card_read(fd);

	for(i = 4; i < BYTE; i++)	{
		size[j] = header[i];
		j++;
	}
	for(i = 0; i < 2; ++i)	{
		temp = size[i];
		size[i] = size[4-i-1];
		size[4-i-1] = temp;
	}
	if(strcmp(file_select,"MUS.WAV") == 0)	{
//	if(file_select == "MUS.WAV")	{
		wav_size = (size[0] << 3*BYTE | size[1] << 2*BYTE | size[2] << BYTE | size[3]) + BYTE;
		printf("The Size of %s is %i\n", file_select, wav_size);
		fflush(stdout);
	}
	else	{
		for(i = 0; i < NumEvents; i++){
			if(Events[i].initialized == 0){
				Events[i].initialized = 1;
				Events[i].filename = file_select;
				break;
			}
		}
		Events[i].size = (size[0] << 3*BYTE | size[1] << 2*BYTE | size[2] << BYTE | size[3]) + BYTE;
		printf("The Size of %s is %i\n", file_select, Events[i].size);
		fflush(stdout);
	}


}

/*
 *@brief Music Enable ISR enables the audio ISR and allows the audio device to write interrupts.
 */
void music_enable_ISR(void){
	int i;
	if(alt_irq_register(AUDIO_IRQ, NULL, audio_isr) == 0){
		printf("IRQ Registered\n");
		fflush(stdout);
	}
	else{
		printf("IRQ Not Registered\n");
		fflush(stdout);
	}
	alt_up_audio_enable_write_interrupt(audio);
		printf("IRQ Write Enabled\n");
		fflush(stdout);

	for(i = 0; i<NumEvents; i++){
		Events[i].music_data_index = 0;
	}
}


/*
 *@brief Audio ISR is the interrupt that is run when the audio interrupt triggers
 *@brief Audio ISR checks if an event sound is to be played, otherwise it plays the background music.
 */
void audio_isr(void * context, alt_u32 id)
{
	int BuffCount;
	if(trigger != -1){
		for(BuffCount = 0; BuffCount < 110; BuffCount++){
				music_buff[BuffCount] = (music_data[music_data_index] >> 1) + (Events[trigger].music_data[Events[trigger].music_data_index]);

				if(music_data_index >= music_data_count)
					music_data_index = 0;
				else
					music_data_index++;
				if(Events[trigger].music_data_index >= Events[trigger].music_data_count){
					Events[trigger].music_data_index = 0;
					trigger = -1;

					printf("Interrupt Triggering!\n");
					fflush(stdout);
				}
				else
					Events[trigger].music_data_index++;
		}
	}
	else{
		fflush(stdout);
		for(BuffCount = 0; BuffCount < 110; BuffCount++)
		{
			music_buff[BuffCount] = music_data[music_data_index];

			if(music_data_index >= music_data_count)
				music_data_index = 0;
			else
				music_data_index++;
		}
	}

	alt_up_audio_write_fifo(audio, music_buff, 110, ALT_UP_AUDIO_LEFT);
	alt_up_audio_write_fifo(audio, music_buff, 110, ALT_UP_AUDIO_RIGHT);
}

/*
 *@brief Music File Load loads the audio file into memory based on the size of the .wav file. The Sample Size is 110.
 */
void music_file_load(void){
	int i,j,l;
	short k;
	unsigned int audio_data[2];
	if(strcmp(file_select,"MUS.WAV") == 0)
		music_data_count = 0;

	music_buff = (unsigned int *) malloc (110 * sizeof(unsigned int));

	if(strcmp(file_select, "MUS.WAV") != 0)
	{
		for(l = 0; l < NumEvents; l++)
		{
			if(file_select == Events[l].filename)
				break;
		}

		Events[l].music_data = (unsigned int *) malloc (Events[l].size/2 * sizeof(unsigned int));

		for(i = 0; i < (Events[l].size/2) ; i++)
		{
			for( j = 0; j < 2; j++)
				audio_data[j] = alt_up_sd_card_read(fd) & (0x00FF) ;

			k = (unsigned char) audio_data[1] << BYTE | (unsigned char) audio_data[0];
			Events[l].music_data[Events[l].music_data_count] = k;

			Events[l].music_data_count++;
		}

		printf("Music Data Count is now at %i, Size is %i \n", Events[l].music_data_count, Events[l].size);
		fflush(stdout);


	}

	else{
		music_data = (unsigned int *) malloc (wav_size/2 * sizeof(unsigned int));

		for(i = 0; i < wav_size/2 ; i++)
		{
			for(j = 0; j < 2; j++)
				audio_data[j] = alt_up_sd_card_read(fd) & (0x00FF);

			k = (unsigned char) audio_data[1] << BYTE | (unsigned char) audio_data[0];

			music_data[music_data_count] = k;

			music_data_count++;

		}
	}
}


