#ifndef __RBOOT_H__
#define __RBOOT_H__

//////////////////////////////////////////////////
// rBoot open source boot loader for ESP8266.
// richardaburton@gmail.com
//////////////////////////////////////////////////

#define CHKSUM_INIT 0xef

#define SECTOR_SIZE 0x1000
#define BOOT_CONFIG_SECTOR 1

#define BOOT_CONFIG_MAGIC 0xe1
#define BOOT_CONFIG_VERSION 0x01

#define MODE_STANDARD 0x00
#define MODE_GPIO_ROM 0x01

// increase if required
#define MAX_ROMS 4

// we can't safely store this where the sdk bootloader does
// (last sector), the last 4 sectors seem to be reserved for
// config but none are safe for our use, they all get written
// over at some point
typedef struct {
	uint8 magic;		   // our magic
	uint8 version;		   // config struct version
	uint8 mode;			   // boot loader mode
	uint8 current_rom;	   // currently selected rom
	uint8 gpio_rom;		   // rom to use for gpio boot
	uint8 count;		   // number of roms in use
	uint8 unused[2];	   // padding
	uint32 roms[MAX_ROMS]; // flash addresses of the roms
	uint8 chksum;		   // config chksum
} rboot_config;

#endif