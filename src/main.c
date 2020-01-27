#include "stm32f0xx.h" 
#include "setup.h"
#include "Main_thread.h"
#include "Linear_func.h"
#include "Flash_msp.h"
#include "usart.h"



int main(void)
{
	system_init();
	USART_Config(2000000);
	char str[32]; 
	sprintf(str, "Boot\n");
	USART_Puts(str);
	while(1)
	{
		Watch_dog_reload();
		do_modbus();
		Main_thread();
		Flash_thread();
	}
}



 