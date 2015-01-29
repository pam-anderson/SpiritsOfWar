/*
 * SoW_sdcard.c
 *
 *  Created on: 2015-01-27
 *      Author: Arjan
 */


#include "SoW.h"

alt_up_sd_card_dev* sdcard;

void sdcard_init() {
	sdcard = alt_up_sd_card_open_dev("/dev/sdcard");
}

int sdcard_write_file(char* file_name, alt_u8* buffer, int size) {
	if(alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16() && sdcard != NULL) {
		int fd = alt_up_sd_card_fopen(file_name, TRUE);
		int i = 0;
		int temp = 0;
		while(i < size && temp >= 0) {
			temp = buffer[i];
			alt_up_sd_card_write(fd, buffer[i]);
			i++;
		}
		alt_up_sd_card_fclose(fd);
	}
	else {
		return -1;
	}
	return 1;
}

int sdcard_read_file(char* file_name, alt_u8* buffer, int size) {
	if(alt_up_sd_card_is_Present() && alt_up_sd_card_is_FAT16() && sdcard != NULL) {
		int fd = alt_up_sd_card_fopen(file_name, FALSE);
		printf("FD IS %x\n", fd);
		int i = 0;
		int temp = 0;
		while(i < size && temp >= 0) {
			temp = alt_up_sd_card_read(fd);
			buffer[i] = temp;
			i++;
		}
		alt_up_sd_card_fclose(fd);
	}
	else {
		return -1;
	}
	return 1;
}

unsigned int sdcard_audio_read(short int fd)
{
	unsigned int buffer;
	unsigned int buffer2;
	buffer = 0;


	buffer = alt_up_sd_card_read(fd);
	buffer = buffer % 100;
	buffer = buffer << 8;
	buffer2 = alt_up_sd_card_read(fd);
	buffer2 = buffer % 100;
	buffer = buffer | buffer2;

	printf("%x", buffer);
	return buffer;
}

int sd_card_audio_open(char * file_name)
{
	return alt_up_sd_card_fopen(file_name, FALSE);
}

void sd_card_audio_close(short int fd)
{
	alt_up_sd_card_fclose(fd);
}
