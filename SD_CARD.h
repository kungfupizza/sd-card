/*
 * SD_CARD.h
 * SD_CARD driver function declaration
 * Created: 10/26/2018 4:28:49 PM
 *  Author: Sarthak
 */ 


#ifndef SD_CARD_H_
#define SD_CARD_H_
#include <asf.h>

// no_file: number of files in SD_CARD
uint8_t no_file;

void sd_card_init(void);

uint8_t sd_file_open(FIL *file_object, FATFS *fs,DIR *dir_object, const char *file_name,uint8_t mode);

int sd_file_write(FIL *file_object,char *data);

void sd_file_list(DIR *dir_object);

uint8_t sd_card_format(FATFS *fs);

uint8_t sd_card_check(void);

FRESULT f_exist (const TCHAR*);

FRESULT sd_file_close (FIL *file_object);

#endif /* SD_CARD_H_ */