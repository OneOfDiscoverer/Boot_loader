#include "Linear_func.h"
#include "setup.h"
#include "MEM_map.h"
#include "gpio_init.h"
#include "Flash_msp.h"

volatile uint16_t led_cnt_0, led_cnt_1, led_cnt_2, cnt_timer;

void system_init(void)
{
	Watch_dog_init();
	SysClockConfig();
	RCC_init();
//	Rcc_reset();	
//	CRC_init();
	GPIO_init();
//	check_firm();
	FT_unlock();
	SysTick_init();
}

void SysClockConfig(void)
{
	uint32_t HSEStartupTimeout = 1000000L;
	
	RCC->CR |= RCC_CR_HSEON;
	while(HSEStartupTimeout-- && !(RCC->CR & RCC_CR_HSERDY));
	
	if (RCC->CR & RCC_CR_HSERDY)
	{
		RCC->CFGR &= ~RCC_CFGR_HPRE;										// AHB not prescaled
		RCC->CFGR &= ~RCC_CFGR_PPRE;										// PCLK = APB
		RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE);
		RCC->CFGR |= RCC_CFGR_PLLMUL6 | RCC_CFGR_PLLSRC_HSE_PREDIV;			// PLL mul = 6 (8x6 = 48MHz)
		RCC->CR |= RCC_CR_PLLON;
		
		while(!(RCC->CR & RCC_CR_PLLRDY));
		
		RCC->CFGR &= ~(RCC_CFGR_SW);
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	}
}

void jump_to_appl(void) 
{
//  __disable_irq();
//  
//  volatile uint32_t *RAMVectorTable = (volatile uint32_t *)RAM_START_ADDRESS;
//  for(uint32_t iVector = 0; iVector < 48; iVector++) 
//      RAMVectorTable[iVector] = *(__IO uint32_t *)((uint32_t)MAIN_PROGRAM_START_ADDRESS + (iVector << 2));
//	
//  SYSCFG->CFGR1 |= SYSCFG_CFGR1_MEM_MODE;
//	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
  
  uint32_t ApplJumpAddr = *(__IO uint32_t*)(MAIN_PROGRAM_START_ADDRESS + 4);
  pfunc_jump_to_appl FuncJumpToAppl = (pfunc_jump_to_appl)ApplJumpAddr;
  __set_MSP(*(__IO uint32_t *)MAIN_PROGRAM_START_ADDRESS);
	
//	__enable_irq();
	
  FuncJumpToAppl(); 
}

void HardFault_Handler(void)
{
	NVIC_SystemReset();
}

void check_firm(void)
{
	if(!(*(uint32_t *)BOOT_LOADER_DATA_PAGE == KEY_UPDATE) && (*(uint32_t *)MAIN_PROGRAM_START_ADDRESS >=0x20000000 && *(uint32_t *)MAIN_PROGRAM_START_ADDRESS <=0x20002000))
	{
		if(crc_calc((uint32_t*)(MAIN_PROGRAM_START_ADDRESS), (uint32_t*)(MAIN_PROGRAM_START_ADDRESS + LENGHT)))
			jump_to_appl();
	}
}

uint8_t crc_calc(uint32_t* addr, uint32_t* end)
{
	CRC->CR = CRC_CR_RESET | CRC_CR_REV_IN_0 | CRC_CR_REV_OUT; //CRC-32/JAMCRC, need xor for compabile with casual crc32
	if(addr != end)
	{
		while	(addr != end)
		{
			CRC->DR = *addr;
			addr++;
		}
		if(!CRC->DR) return 1;
	}
	return 0;
}

void Set_link_cnt(uint16_t cnt, uint8_t n)		//set led on duration
{
	switch(n)
	{
		case 0: led_cnt_0 = cnt; LED0ON(); break;
		case 1: led_cnt_1 = cnt; LED1ON(); break;
		case 2: led_cnt_2 = cnt; LED2ON(); break;
	}
}

void Set_link_mb_cnt(uint16_t cnt)		//set led on duration
{
		led_cnt_1 = cnt; LED1ON();
		led_cnt_2 = cnt; LED2ON();
}

void SysTick_Handler(void)				//handler systick
{
	if(cnt_timer) 	cnt_timer--;
	else
	{	
		Set_link_cnt(150,0);
		Set_link_cnt(150,2);
		cnt_timer = 1000; 
	}
	if(led_cnt_0) led_cnt_0--;
	else LED0OFF();
	if(led_cnt_1) led_cnt_1--;
	else LED1OFF(); 
	if(led_cnt_2) led_cnt_2--;
	else LED2OFF();
}

void RCC_init(void)
{
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_PLLON);
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE);
	RCC->CFGR |= (RCC_CFGR_PLLSRC);
	
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    
	
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1);
	
	//RCC->CSR |= RCC_CSR_RMVF;
}
void CRC_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
}

void GPIO_init(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 
	gpio_init(GPIOB, 0, MODE_output, OTYPE_pp, OSPEED_low, PUPD_no, AF_0);
	gpio_init(GPIOB, 1, MODE_output, OTYPE_pp, OSPEED_low, PUPD_no, AF_0);
	gpio_init(GPIOA, 11, MODE_output, OTYPE_pp, OSPEED_low, PUPD_no, AF_0);
}

void SysTick_init(void)						//init systick
{
	SystemCoreClockUpdate();
	SysTick->LOAD=SystemCoreClock/1000;
	SysTick->VAL=0;
	SysTick->CTRL=SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;
}

void Rcc_reset(void)
{
		RCC -> AHBRSTR |= RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST;	
		RCC -> APB1RSTR |=  RCC_APB1RSTR_USART2RST;
		RCC -> APB2RSTR |=	RCC_APB2RSTR_USART1RST;
		uint16_t i = 0xFFFF;
		while(i)	i--;
		RCC -> AHBRSTR &= ~(RCC_AHBRSTR_GPIOARST | RCC_AHBRSTR_GPIOBRST);
		RCC -> APB1RSTR &= ~RCC_APB1RSTR_USART2RST; 
		RCC -> APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
		i = 0xFFFF;
		while(i)	i--;
}

void Watch_dog_init(void)
{
	IWDG -> KR = 0x0000CCCC;
	IWDG -> KR = 0x00005555;
	IWDG -> PR = 7;
	IWDG -> RLR = 0xFFFFFFFF;
	while(!(IWDG->SR == 0x00000000));
}

void Watch_dog_reload(void)
{
	IWDG -> KR = 0x0000AAAA;	
}




