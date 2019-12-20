#ifndef _GPIO_INIT
#define _GPIO_INIT

#include "stdint.h"
#include "stm32f030x8.h"
//#include "stm32l0xx.h"
//#include "stm32l051xx.h"
//#include "stm32f051x8.h"

void gpio_init(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num, uint8_t gpio_moder, uint8_t gpio_otyper, uint8_t gpio_ospeedr, uint8_t gpio_pupdr, uint8_t gpio_afr);
void gpio_switch(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num, uint8_t state);
uint8_t gpio_get_state(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num);

#ifndef TRUE
#define TRUE					1
#endif

#ifndef FALSE
#define FALSE					0
#endif

#define GP_INP				MODE_input, OTYPE_pp,OSPEED_low, PUPD_no, AF_0
#define GP_OUT				MODE_output, OTYPE_pp,OSPEED_fast, PUPD_no, AF_0

#define P_0						0
#define P_1						1
#define P_2						2
#define P_3						3
#define P_4						4
#define P_5						5
#define P_6						6
#define P_7						7
#define P_8						8
#define P_9						9
#define P_10					10
#define P_11					11
#define P_12					12
#define P_13					13
#define P_14					14
#define P_15					15


#define MODE_input 		0
#define MODE_output 	1
#define MODE_af				2
#define MODE_analog		3

#define OTYPE_pp			0
#define OTYPE_od			1

#define OSPEED_low		0
#define OSPEED_mid		1
#define OSPEED_fast		2
#define OSPEED_vfast	3

#define PUPD_no				0
#define PUPD_pu				1
#define PUPD_pd				2

#define AF_0					0
#define AF_1					1
#define AF_2					2
#define AF_3					3
#define AF_4					4
#define AF_5					5
#define AF_6					6
#define AF_7					7
#define AF_8					8
#define AF_9					9
#define AF_10					10
#define AF_11					11
#define AF_12					12
#define AF_13					13
#define AF_14					14
#define AF_15					15

#endif
