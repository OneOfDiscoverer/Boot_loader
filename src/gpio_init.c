#include "gpio_init.h"


void gpio_init(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num, uint8_t gpio_moder, uint8_t gpio_otyper, uint8_t gpio_ospeedr, uint8_t gpio_pupdr, uint8_t gpio_afr)
{	
	gpio_pnt->MODER &= ~(GPIO_MODER_MODER0 << (gpio_num * 2)); //GPIO_MODER_MODER0 !!!
	gpio_pnt->MODER |= gpio_moder << (gpio_num * 2);
	
	gpio_pnt->OTYPER &= ~(GPIO_OTYPER_OT_1 << gpio_num);
	gpio_pnt->OTYPER |= gpio_otyper << gpio_num;
	
	gpio_pnt->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (gpio_num * 2)); //GPIO_OSPEEDER_OSPEEDR0!!
	gpio_pnt->OSPEEDR |= gpio_ospeedr << (gpio_num * 2);
	
	gpio_pnt->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (gpio_num * 2)); //GPIO_PUPDR_PUPD0!!
	gpio_pnt->PUPDR |= gpio_pupdr << (gpio_num * 2);
		
	gpio_pnt->AFR[(gpio_num > 7)] &= ~(GPIO_AFRL_AFSEL0_Msk << ((gpio_num - 8*(gpio_num > 7)) * 4));
	gpio_pnt->AFR[(gpio_num > 7)] |= gpio_afr << ((gpio_num - 8*(gpio_num > 7)) * 4);
}

void gpio_switch(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num, uint8_t state)
{
	gpio_pnt->BSRR |= 1 << (gpio_num + 16*(!state));
}

uint8_t gpio_get_state(GPIO_TypeDef *gpio_pnt, uint8_t gpio_num)
{
	return (uint8_t)(gpio_pnt -> IDR && (1 << gpio_num));
}

