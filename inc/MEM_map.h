#ifndef __MEM_MAP_H__
#define __MEM_MAP_H__

#define MAIN_PROGRAM_START_ADDRESS					(0x08002000)
#define	BOOT_LOADER_START_ADDRESS						(0x08000000)
#define RAM_START_ADDRESS										(0x20000000)
#define FLASH_END_AREA											(0x08010000)
#define LENGHT															(0xFFFF)
#define BOOT_DATA_VOLUME										(0x0400)
#define MAIN_DATA_VOLUME										(0x0800)

#define BOOT_LOADER_DATA_PAGE								(MAIN_PROGRAM_START_ADDRESS - BOOT_DATA_VOLUME)
#define MAIN_PROGRAM_DATA_PAGE							(FLASH_END_AREA - MAIN_DATA_VOLUME)

#define KEY_UPDATE													(0xDEAD)

#endif
