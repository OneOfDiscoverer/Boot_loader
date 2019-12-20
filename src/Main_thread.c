#include "Main_thread.h"
#include "stm32f0xx.h"
#include "setup.h"
#include "Flash_msp.h"
#include "Linear_func.h"
#include "MEM_map.h"
#include "usart.h"
#include "string.h"

extern void Set_link_cnt(uint16_t cnt, uint8_t n);
extern volatile uint8_t buf[BUF_LEN];
extern volatile uint16_t len;

uint16_t num_page;
uint8_t MT_mode;

void up_mt(void)
{
	MT_mode = MT_update_f;
}
 
void Main_thread(void)
{
	char str[32];
	Watch_dog_reload();
	if(FT_get_state() == FT_wait)
		switch(MT_mode)
		{
			case MT_wait:
				break;
			case MT_reset:
				NVIC_SystemReset();
				break;
			case MT_update_f:
				if(!strcmp((char*)"restart\n", (char*)buf)) NVIC_SystemReset();
				else if(!strcmp((char*)"jump\n", (char*)buf)) jump_to_appl();
				else if(!strcmp((char*)"flash\n", (char*)buf)) 
				{
					MT_mode = MT_data_ready_f;
					num_page = 0;
				}
				else MT_mode = MT_wait;
				break;
			case MT_data_ready_f:		
				if(crc_calc((uint32_t*)&buf,  (uint32_t*)(&buf + BUF_LEN)) && len == BUF_LEN) 
				{
					FT_set_erase((uint32_t*)MAIN_PROGRAM_START_ADDRESS);
					MT_mode = MT_data_ready_s;
				}
				else if(!len && len !=BUF_LEN) 
				{
					sprintf(str, "Frame error\n");
					USART_Puts(str); 
					MT_mode = MT_wait;
				}
				break;
			case MT_data_ready_s:
				FT_set_write((uint32_t*)MAIN_PROGRAM_START_ADDRESS + num_page*0x400, (uint32_t*)MAIN_PROGRAM_START_ADDRESS + num_page*0x400 + 0x400, (uint32_t*)&buf);
				num_page++;
				sprintf(str, "Page %d done\n", num_page);
				USART_Puts(str);
				MT_mode = MT_wait;
				break;
		}
}


//			case MT_update_f:
//				FT_set_erase((uint16_t*)BOOT_LOADER_DATA_PAGE);
//				MT_mode = MT_update_s;
//				break;
//			case MT_update_s:
//				FT_set_write(	(uint16_t*)BOOT_LOADER_DATA_PAGE+1, (uint16_t*)BOOT_LOADER_DATA_PAGE+7, (uint16_t*)&ext_start_addr_l);
//				ext_command_reg &= ~(_COM_UPDATE);
//				MT_mode = MT_wait;
//				break;
//			case MT_data_ready_f:
//				ext_end_addr_h = ext_start_addr_h;
//				ext_end_addr_l = ext_start_addr_l+_PAGE8_LENGHT;
//				if(crc_calc((uint32_t*)&ext_data_page, ((uint32_t*)&ext_data_page+_BUF32_LENGHT)))
//				{
//					ext_command_reg &= ~(_FLAG_CRC_MB_BUF);
//					FT_set_erase((uint16_t*)(ext_start_addr_h << 16 | ext_start_addr_l));
//					MT_mode = MT_data_ready_s;
//				}
//				else
//				{
//					ext_command_reg |= _FLAG_CRC_MB_BUF;
//					ext_command_reg &= ~(_COM_DATA_READY);
//					MT_mode = MT_wait;
//				}
//				break;
//			case MT_data_ready_s:
//				FT_set_write(	(uint16_t*)(ext_start_addr_h << 16 | ext_start_addr_l),
//											(uint16_t*)(ext_end_addr_h << 16 | ext_end_addr_l),
//											(uint16_t*)&ext_data_page);
//				MT_mode = MT_calc_all;
//				break;
//			case MT_calc_all:
//				if(crc_calc((uint32_t*)(ext_start_addr_h << 16 | ext_start_addr_l), 
//					(uint32_t*)(ext_end_addr_h << 16 | ext_end_addr_l)))
//					ext_command_reg &= ~(_FLAG_CRC_FLASH);
//				else 	
//					ext_command_reg |= _FLAG_CRC_FLASH;
//				ext_command_reg &= ~(_COM_DATA_READY | _COM_CALC_ALL);
//				MT_mode = MT_wait;
