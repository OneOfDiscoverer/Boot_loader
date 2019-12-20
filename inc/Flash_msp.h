#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f0xx.h"
#include "stdint.h"

#define FLASH_FKEY1				0x45670123
#define FLASH_FKEY2				0xCDEF89AB 



void FT_unlock(void);
void FT_set_write(uint32_t* addr_st, uint32_t* addr_en, uint32_t* data);
void FT_set_erase(uint32_t* addr);
void Flash_thread(void);
uint32_t FT_get_state(void);

enum flash_thread
	{
		FT_wait,
		FT_ready,
		FT_erase,
		FT_erase_done,
		FT_write,
		FT_write_done,
	};

#endif
