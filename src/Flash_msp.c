#include "Flash_msp.h"
#include "stm32f0xx.h"
#include "setup.h"
#include "MEM_map.h"

volatile uint16_t FT_mode, *FT_addr, *FT_end_addr;
volatile uint16_t *FT_data;

void FT_unlock(void)
{
	FLASH->KEYR = FLASH_FKEY1;
	FLASH->KEYR = FLASH_FKEY2; 
}
uint32_t FT_get_state(void)
{
	return FT_mode;
}

void FT_set_erase(uint16_t* addr)
{
	FT_mode = FT_erase;
	FT_addr = addr;
}
void FT_set_write(uint16_t* addr_st, uint16_t* addr_en, uint16_t* data)
{
	FT_mode = FT_write;
	FT_addr = addr_st;
	FT_end_addr = addr_en;
	FT_data = data; 
}

void Flash_thread(void)
{
	static uint32_t shift_addr = 0;
	switch(FT_mode)
	{
		case FT_wait:
			break;
		case FT_ready:
			if(FLASH->SR & FLASH_SR_EOP)
				FT_mode = FT_wait;
			break;
		case FT_erase:
			FLASH->CR |= FLASH_CR_PER;  
			FLASH->AR =  (uint32_t)FT_addr;  
			FLASH->CR |= FLASH_CR_STRT;   
			FT_mode = FT_erase_done;
			break;
		case FT_erase_done:
			if(FLASH->SR & FLASH_SR_EOP)
			{
				FLASH->SR = FLASH_SR_EOP; 
				FLASH->CR &= ~FLASH_CR_PER;
				shift_addr = 0;
				FT_mode = FT_wait;
			}
			break;
		case FT_write:
			FLASH->CR |= FLASH_CR_PG; 
			*(FT_addr+shift_addr) = *(FT_data+shift_addr);
			FT_mode = FT_write_done;
			break;
		case FT_write_done:
			if(FLASH->SR & FLASH_SR_EOP)
			{
				FT_mode = FT_write;
				FLASH->SR |= FLASH_SR_EOP;
				FLASH->CR &= ~FLASH_CR_PG;
				shift_addr++;
				if((FT_addr+shift_addr) == FT_end_addr)
				{
					shift_addr = 0;
					FT_mode = FT_wait;
				}


			}
			break;
	}
}



