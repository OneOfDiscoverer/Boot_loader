#ifndef __LINEAR_FUNC_H__
#define __LINEAR_FUNC_H__

#include "stm32f0xx.h"
#include "stdint.h"

typedef void (*pfunc_jump_to_appl)(void);

void HardFault_Handler(void);
void Rcc_reset(void);
void GPIO_init(void);
void SysTick_Handler(void);
void SysTick_init(void);
void Set_link_cnt(uint16_t cnt, uint8_t n);
void CRC_init(void);
void remapMemToSRAM(void);
void GoMainApp(void);
void jump_to_appl(void);
void enter_boot(void);
void check_firm(void);
void RCC_init(void);
uint8_t crc_calc(uint32_t* addr, uint32_t* end);
void Watch_dog_init(void);
void Watch_dog_reload(void);
void system_init(void);
void SysClockConfig(void);
#endif
