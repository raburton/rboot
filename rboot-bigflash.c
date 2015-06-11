//////////////////////////////////////////////////
// rBoot open source boot loader for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//////////////////////////////////////////////////

typedef unsigned int uint32;
typedef unsigned char uint8;

#include "rboot.h"

#ifdef BOOT_BIG_FLASH

uint8 rBoot_mmap_1 = 0xff;
uint8 rBoot_mmap_2 = 0xff;

// this function must remain in iram - DO NOT mark with ICACHE_FLASH_ATTR
void Cache_Read_Enable_New() {
	
	if (rBoot_mmap_1 == 0xff) {
		uint32 addr;
		rboot_config conf;
		
		Cache_Read_Disable();
		
		SPIRead(BOOT_CONFIG_SECTOR * SECTOR_SIZE, &conf, sizeof(rboot_config));
		
		addr = conf.roms[conf.current_rom];
		addr /= 0x100000;
		
		rBoot_mmap_2 = addr / 2;
		rBoot_mmap_1 = addr % 2;
		
		//ets_printf("mmap %d,%d,1\r\n", rBoot_mmap_1, rBoot_mmap_2);
	}
	
	Cache_Read_Enable(rBoot_mmap_1, rBoot_mmap_2, 1);
}

#endif
