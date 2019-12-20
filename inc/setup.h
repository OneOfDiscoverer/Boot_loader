#ifndef __SETUP_H__
#define __SETUP_H__

#define DEVICE_ID						(0x1ffff7f8)

#define LED_DURATION 1000
#define LED0ON() 	GPIOB ->BSRR = GPIO_BSRR_BS_0
#define LED1ON() 	GPIOB ->BSRR = GPIO_BSRR_BS_1
#define LED2ON()	GPIOA ->BSRR = GPIO_BSRR_BS_11
#define LED0OFF() GPIOB ->BSRR = GPIO_BSRR_BR_0
#define LED1OFF() GPIOB ->BSRR = GPIO_BSRR_BR_1
#define LED2OFF()	GPIOA ->BSRR = GPIO_BSRR_BR_11

//default values
#define _DEF_ADDR 					99

//3x out reg
#define ext_sec_cnt 				(buf_3x[0])    	
#define ext_min_cnt 				(buf_3x[1])    	
#define ext_hour_cnt 				(buf_3x[2])    	
#define ext_timing 					(buf_3x[3])    	
#define ext_usart_ch				(buf_3x[4])			
#define ext_mb_addr 				(buf_3x[5])    	
#define ext_mb_status 			(buf_3x[6])			
#define ext_mb_crc_err 			(buf_3x[7])   	
#define ext_firm_ver 				(buf_3x[8])			
#define ext_ret_crc_l				(buf_3x[11])
#define ext_ret_crc_h				(buf_3x[12])
#define ext_chip_id					(buf_3x[13])
#define ext_reset_state			(buf_3x[18])
#define ext_ow_device_cnt		(buf_3x[31])    
#define ext_ow_data_reg			(buf_3x[32])    

//4x inout reg
#define ext_command_reg			(buf_4x[0])			
#define ext_start_addr_l		(buf_4x[1])			
#define ext_start_addr_h 		(buf_4x[2])			
#define ext_end_addr_l			(buf_4x[3])			
#define ext_end_addr_h			(buf_4x[4])				
#define ext_crc_l						(buf_4x[5])				
#define ext_crc_h						(buf_4x[6])					
#define ext_data_page				(buf_4x[7])					
//bit def

#define _COM_RESET					0x0080
#define _COM_UPDATE					0x0100
#define _COM_DATA_READY			0x0200
#define _COM_CALC_ALL				0x0400
#define _FLAG_CRC_MB_BUF		0x0800
#define _FLAG_CRC_FLASH			0x1000

#define _BOOT_VER						0x0006

#ifdef MB_BUF_SIZE_4x			
#undef MB_BUF_SIZE_4x
#define  MB_BUF_SIZE_4x			(1024)
#endif

#endif

