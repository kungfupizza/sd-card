/*
 * SD_CARD.c
 * SD_CARD driver function
 * Created: 10/26/2018 4:29:12 PM
 *  Author: Sarthak
 */ 

#include "SD_CARD.h"

/******************************  SD CARD INIT  ***********************************************
* Function Name: sd_card_init
* Description: initializes the SPI & SD_CARD
* Parameter: None
* Return value: None
**********************************************************************************************/
void sd_card_init(void)
{
	uint8_t status;
	sd_mmc_init();
	do {
		status = sd_mmc_test_unit_ready(0);
		if (CTRL_FAIL == status) {
			user_debug_1("Card install FAIL\n\r");
			user_debug_1("Please unplug and re-plug the card.\n\r");
			while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
			}
		}
	} while (CTRL_GOOD != status);
	
}

/*****************************************************************************************************************************
*	Check whether SD Card is present or not & returns a value.
*	Return 1(Present) or 0(Not Present)
*****************************************************************************************************************************/
uint8_t sd_card_check(void)
{
	sd_mmc_init();
	return sd_mmc_check(0);
}

/******************************  SD CARD FILE OPEN  ***********************************************
* Function Name: sd_file_open
* Description: It creates or opens the file from SD_CARD depending on the modes. 
* Parameter: FIL *file_object (contains a pointer to file in case of successfully opened)
		   : FATFS *fs (fat file system object)
 		   : DIR *dir_object (register the file in the Directory)
		   : char *file_name (name of the file)
		   : mode: FA_WRITE | FA_READ | FA_CREATE_NEW 
				 : FA_WRITE | FA_READ | FA_OPEN_EXISTING
* Return value: return 0 (SUCCESS)
			  : return 1 (failure)
**********************************************************************************************/
uint8_t sd_file_open(FIL *file_object, FATFS *fs,DIR *dir_object, const char *file_name,uint8_t mode)
{
	FRESULT res;
		
	memset(fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, fs);
	if (FR_INVALID_DRIVE == res) {
		user_debug_1("[FAIL] res %d\r\n", res);
		user_debug_1("Please unplug the card.\n\r");
		return 1;
	}
	user_debug_1(" the card capacity is %ld \r\n",sd_mmc_get_capacity(0));
	res = f_open(file_object,(char const *)file_name,mode);
	if(!res)
	{
		if(mode == (FA_WRITE|FA_CREATE_ALWAYS))
		{
			f_opendir(dir_object,(char const *)file_name);
			no_file++;	
			return 0;
		}
		user_debug_1("file opened successfully\r\n");
		return 0;
	}
	
	return 0;
}

/******************************  SD CARD FILE WRITE  ***********************************************
* Function Name: sd_file_write
* Description: It is used to write the contents to the file. 
* Parameter: FIL *file_object (contains a pointer to file in case of successfully opened)
		   : char *data (character array)
* Return value: return 1 (SUCCESS)
			  : return 0 (failure)
**********************************************************************************************/
int sd_file_write(FIL *file_object,char *data)
{
	int res;
	//file_object->fptr=file_object->fsize;
	f_lseek(file_object,f_size(file_object));
	gpio_set_pin_low(SD_MMC_SPI_0_CS);
	res=f_puts(data, file_object);
	if (res <= 0) {
		f_close(file_object);
		user_debug_1("[FAIL]\r\n");
		user_debug_1("Please unplug the card.\n\r");
		return res;
	}
	return res;
}


/******************************  SD CARD FILE LIST  ***********************************************
* Function Name: sd_file_list
* Description: It is used to list the file names present in SD_CARD. 
* Parameter: DIR *dir_object (contains a pointer to directory entry)
* Return value: return 1 (SUCCESS)
			  : return 0 (failure)
**********************************************************************************************/
void sd_file_list(DIR *dir_object)
{
	FILINFO file_info;
	dir_object->index=0;
	dir_object->dir = dir_object->fs->win + (0 % 16) * 32;

	for(int i=1;i<=no_file;i++)
	{
		
		if(!f_readdir(dir_object,&file_info)){
			
			user_debug_1("i. %s\r\n",file_info.fname);
		}
		else{user_debug_1("not able to read directory \r\n"); }
	}
}

/******************************  SD CARD FORMAT  ***********************************************
* Function Name: sd_card_format
* Description: It is used to format the SD_CARD in FAT format. 
* Parameter: FATFS *fs (fat file system object)
* Return value: return 0 (SUCCESS)
			  : return 1 (failure)
**********************************************************************************************/
uint8_t sd_card_format(FATFS *fs)
{
	uint8_t res;
	
	memset(fs, 0, sizeof(FATFS));
	res = f_mount(LUN_ID_SD_MMC_0_MEM, fs);
	if (FR_INVALID_DRIVE == res) {
		user_debug_1("[FAIL] res %d\r\n", res);
		user_debug_1("Error in formating\r\n");
		return 1;
	}
	gpio_set_pin_low(SD_MMC_SPI_0_CS);
	gpio_set_pin_high(SD_MMC_SPI_0_CS);
	
	return 0;
}

FRESULT f_exist (const TCHAR *path_old)
{
	FRESULT res;
	FILINFO floc;
	floc.lfname=0;
	res = f_stat(path_old,&floc);
	return res;
}


FRESULT sd_file_close (FIL *file_object)
{
	f_close(file_object);
}