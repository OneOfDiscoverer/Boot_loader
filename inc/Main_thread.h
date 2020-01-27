#ifndef __MAIN_THREAD_H__
#define __MAIN_THREAD_H__
#include "stm32f0xx.h"

#define _PAGE8_LENGHT 								0x400
#define _BUF32_LENGHT									0x100

void Main_thread(void);
void up_mt(void);

enum Main_thread
	{
		MT_wait,
		MT_reset,
		MT_update_f,
		MT_update_s,
		MT_data_ready_f,
		MT_data_ready_s,
		MT_data_ready_a,
		MT_data_ready_b,
		MT_data_ready_c,
		MT_data_ready_d,
		MT_calc_all_f,
		MT_calc_all_s,
		MT_jump,
	};

#endif
