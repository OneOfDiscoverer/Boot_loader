#include "Main_thread.h"
#include "stm32f0xx.h"
#include "setup.h"
#include "Flash_msp.h"
#include "Linear_func.h"
#include "MEM_map.h"
#include "usart.h"
#include "string.h"

extern void Set_link_cnt(uint16_t cnt, uint8_t n);
extern volatile uint16_t buf[BUF_LEN];
extern volatile uint16_t len;

uint16_t num_page;
uint8_t MT_mode;

void up_mt(void)
{
	if(MT_mode == MT_wait) MT_mode = MT_update_f;
}
 
void Main_thread(void)
{
	char str[32];
	static uint32_t addl, addh, crc;
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
				switch((uint8_t)buf[0])
				{
					case 0xDE:
						NVIC_SystemReset();
						break;
					case 0x55:
						sprintf(str, "Boot\n");
						MT_mode = MT_wait;
						break;
					case 0x01:
						jump_to_appl();
						break;
					case 0x02:
						MT_mode = MT_update_s;
						num_page = 0;
						sprintf(str, "ready\n");
						break;
					case 0x04:
						MT_mode = MT_calc_all_f;
						num_page = 0;
						sprintf(str, "ready\n"); 
						break;
					default:
						if(len == BUF_LEN) sprintf(str, "Overrun\n");
						else sprintf(str, "Com err\n");
						MT_mode = MT_wait;
						break;
				}
				buf_erase();
				USART_Puts(str); 
				break;
			case MT_update_s:
				if(len) 
				{
					mb_wait();
					*(&addl) = *(uint16_t*)&buf[0];
					*(&addh) = *(uint16_t*)&buf[2];
					sprintf(str, "addr get\n");
					USART_Puts(str); 
					buf_erase();
					MT_mode = MT_data_ready_f;
				}
				break;
			case MT_data_ready_f:		
				if(len)	
				{
					mb_wait();
					*(&crc) = *(uint32_t*)&buf[0];
					sprintf(str, "crc get\n");
					USART_Puts(str); 
					buf_erase();
					MT_mode = MT_data_ready_a;
				}
				break;
			case MT_data_ready_a:
				if(len == 1024) 
				{
					MT_mode = MT_data_ready_b;
				}
				break;
			case MT_data_ready_b:
			{
				uint32_t tmp;
				tmp = crc_calc((uint32_t*)&buf,  (uint32_t*)&buf + 0x100);
				
				if(tmp == crc) 
				{
					FT_set_erase((uint16_t*)addl);
					MT_mode = MT_data_ready_s;
				}
				else
				{
					sprintf(str, "crc %X err %X\n", crc, tmp);
					USART_Puts(str); 
					MT_mode = MT_wait;
				}
			}
				break;
			case MT_data_ready_s:
				FT_set_write((uint16_t*)addl, (uint16_t*)addh, (uint16_t*)buf);
				MT_mode = MT_data_ready_c;
				break;
			case MT_data_ready_c:
				sprintf(str, "Page writed\n");
				buf_erase();
				USART_Puts(str);
				MT_mode = MT_wait;
				break;
			case MT_calc_all_f:
				if(len)	
				{
					mb_wait();
					*(&addl) 	= *(uint32_t*)&buf[0];
					*(&addh) 	= *(uint32_t*)&buf[2];
					*(&crc) 	= *(uint32_t*)&buf[4];
					FT_set_erase((uint16_t*)BOOT_LOADER_DATA_PAGE);	
					sprintf(str, "crc get\n");
					USART_Puts(str); 
					MT_mode = MT_calc_all_s;
				}
				break;
			case MT_calc_all_s:
				{
					uint32_t tmp = crc_calc((uint32_t*)addl, (uint32_t*)addh);
					if(crc == tmp)
					{
						sprintf(str, "crc right\n");
						USART_Puts(str); 
						uint16_t buf_flash[6];
						
						*(uint32_t*)&buf_flash[0] = *(&addl);
						*(uint32_t*)&buf_flash[2] = *(&addh);
						*(uint32_t*)&buf_flash[4] = *(&crc);
						FT_set_write((uint16_t*)BOOT_LOADER_DATA_PAGE+1, (uint16_t*)BOOT_LOADER_DATA_PAGE+7, (uint16_t*)&buf_flash[0]);
					}
					else 
					{
						sprintf(str, "!crc\n");
						USART_Puts(str); 					
					}
					MT_mode = MT_jump;
					buf_erase();
				}
				break;
			case MT_jump:
				NVIC_SystemReset();
				break;
		}
}

