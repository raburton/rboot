//////////////////////////////////////////////////
// rBoot open source boot loader for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//////////////////////////////////////////////////

#include <rboot.h>

#ifdef BOOT_BIG_FLASH

// plain sdk defaults to iram
#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void Cache_Read_Disable(void);
extern uint32 SPIRead(uint32, void*, uint32);
extern void ets_printf(const char*, ...);
extern void Cache_Read_Enable(uint32, uint32, uint32);

uint8 rBoot_mmap_1 = 0xff;
uint8 rBoot_mmap_2 = 0xff;

#ifdef BOOT_RTC_ENABLED
typedef union
{
	rboot_rtc_data	data;
	uint32_t 		overlay[4];
} rboot_rtc_data_overlay_t;
#endif

// this function must remain in iram
void IRAM_ATTR Cache_Read_Enable_New(void) {
	
	if (rBoot_mmap_1 == 0xff) {
		uint32 val;
		rboot_config conf;

		SPIRead(BOOT_CONFIG_SECTOR * SECTOR_SIZE, &conf, sizeof(rboot_config));

#ifdef BOOT_RTC_ENABLED
		{
			const rboot_rtc_data_overlay_t *rtc_in_iospace;
			rboot_rtc_data_overlay_t rtc_in_dram;
			unsigned int ix;

			rtc_in_iospace = (const rboot_rtc_data_overlay_t *)(0x60001100 + (RBOOT_RTC_ADDR * 4));

			for(ix = 0; ix < sizeof(rtc_in_dram.overlay) / sizeof(rtc_in_dram.overlay[0]); ix++)
				rtc_in_dram.overlay[ix] = rtc_in_iospace->overlay[ix];

			// Don't check for next_mode == RBOOT_TEMP_ROM and neither use next_slot
			// 		because they already have been reset by rboot at this point.
			// Trust rboot to have selected the correct rom slot instead.

			if(rtc_in_dram.data.magic == RBOOT_RTC_MAGIC)
				val = conf.roms[rtc_in_dram.data.last_rom];
			else
				val = conf.roms[conf.current_rom];
		}
#else
		val = conf.roms[conf.current_rom];
#endif

		val /= 0x100000;

		rBoot_mmap_2 = val / 2;
		rBoot_mmap_1 = val % 2;
		
		//ets_printf("mmap %d,%d,1\r\n", rBoot_mmap_1, rBoot_mmap_2);
	}
	
	Cache_Read_Enable(rBoot_mmap_1, rBoot_mmap_2, 1);
}

#ifdef __cplusplus
}
#endif

#endif

