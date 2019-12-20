#include "stm32f0xx.h"
#include "usart.h"
#include "Main_thread.h"


volatile uint8_t buf[BUF_LEN];
volatile uint16_t len;

void USART_Config(uint32_t baudRate) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_DMAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2_Msk | GPIO_AFRL_AFRL3_Msk);
	GPIOA->AFR[0] |= (1L << GPIO_AFRL_AFSEL2_Pos) | (1L << GPIO_AFRL_AFSEL3_Pos);
	SystemCoreClockUpdate();
	
	USART2->CR1 = 0;
	USART2->CR2 = 0;
	USART2->CR3 = USART_CR3_DMAT | USART_CR3_DMAR;
	USART2->BRR = SystemCoreClock / baudRate;
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;// | USART_CR1_RXNEIE; // rx dma ch5

	
	DMA1_Channel5->CMAR = (uint32_t)buf;
	DMA1_Channel5->CCR = DMA_CCR_MINC;// | DMA_CCR_MSIZE_1;// | DMA_CCR_DIR;
	DMA1_Channel5->CPAR = (uint32_t)&USART2->RDR;
	
	DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_DIR;
	DMA1_Channel4->CPAR = (uint32_t)&USART2->TDR;
	
//	NVIC_EnableIRQ(USART2_IRQn);
}
void USART_Putchar(uint8_t d) {
	while(!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = d;
}

void do_modbus(void)
{
	static uint16_t state, tmo;
	switch(state)
	{
		case MB_STATE_START_WAIT:
		{
			if(USART2->ISR & USART_ISR_RXNE)
			{
				DMA1->IFCR |= DMA_IFCR_CTCIF5|DMA_IFCR_CGIF5|DMA_IFCR_CHTIF5|DMA_IFCR_CTEIF5;
				DMA1_Channel5->CNDTR = BUF_LEN;
				DMA1_Channel5->CCR |= DMA_CCR_EN;								// ???????? DMA ?? ????? ??????
				tmo = 0;
				len = 0;
				state = MB_STATE_WAIT;
			}
			break;
		}
		case MB_STATE_WAIT:
		{
			if(USART2->ISR & USART_ISR_IDLE) tmo++;
			if (DMA1->ISR & DMA_ISR_TCIF5)
			{
				state = MB_STATE_START_WAIT;
				len = BUF_LEN;
			}
			if (tmo >= 0xFFFF)
			{
				USART2->ICR |= USART_ICR_RTOCF;
				len = BUF_LEN - DMA1_Channel5->CNDTR;
				DMA1_Channel5->CCR &= ~DMA_CCR_EN;
				up_mt();
				state = MB_STATE_START_WAIT;
			}
			break;
		}
	}
}

void USART2_IRQHandler(void)
{
	DMA1->IFCR |= DMA_IFCR_CTCIF5 | DMA_IFCR_CGIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CTEIF5;
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	DMA1_Channel5->CNDTR = USART2->RDR;
	DMA1_Channel5->CCR |= DMA_CCR_EN;
	while(!(DMA1->ISR & DMA_ISR_TCIF5));
}

void USART_Puts(char *s) {
	uint16_t len = 0;
	DMA1_Channel4->CMAR = (uint32_t)s;
	while(*s++) len++;
	DMA1->IFCR |= DMA_IFCR_CTCIF4 | DMA_IFCR_CGIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4;
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CNDTR = len;
	DMA1_Channel4->CCR |= DMA_CCR_EN;
	while(!(DMA1->ISR & DMA_ISR_TCIF4));	
}
void USART_PutDat(uint32_t s) {
	uint16_t len = 1;
	DMA1_Channel4->CMAR = (uint32_t)s;
	DMA1->IFCR |= DMA_IFCR_CTCIF4 | DMA_IFCR_CGIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4;
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CNDTR = len;
	DMA1_Channel4->CCR |= DMA_CCR_EN;
	while(!(DMA1->ISR & DMA_ISR_TCIF4));	
}
