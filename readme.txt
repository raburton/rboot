rBoot - An open source boot loader for the ESP8266
--------------------------------------------------
by Richard A Burton, richardaburton@gmail.com
http://richard.burtons.org/


rBoot is designed to be a flexible open source boot loader, a replacement for
the binary blob supplied with the SDK. It has the following advantages over the
Espressif loader:

  - Open source (written in C).
  - Supports up to 256 roms.
  - Roms can be variable size.
  - Able to test multiple roms to find a valid backup (without resetting).
  - Flash layout can be changed on the fly (with care and appropriately linked
    rom images).
  - GPIO support for rom selection.
  - Reserves less ram (16 bytes vs. 144 bytes for the SDK loader).
  - Documented config structure to allow easy editing from user code.

Building
--------
Makefile is included, which should work with the gcc xtensa cross compiler.
There are two source files, the first is compiled and included as data in the
second. When run this code is copied to memory and executed (there is a good
reason for this, see my blog for an explanation). The make file will handle this
for you, but you'll need my esptool2 (see github).

Installation
------------
Simply write rboot.rom to the first sector of the flash. Remember to set your
flash size correctly with your chosen flash tool (e.g. for esptool.py use the
-fs option). When run rBoot will create it's own config at the start of sector
two for a simple two rom system. You can can then write your two roms to flash
addresses 0x2000 and (half chip size + 0x2000). E.g. for 8Mbit flash:
  esptool.py -fs 8m 0x0000 rboot.rom 0x2000 user1.rom 0x82000 user2.rom

For more interesting rom layouts you'll need to write an rBoot config sector
manually, see next step.

rBoot Config
------------
typedef struct {
	uint8 magic;           // our magic
	uint8 version;         // config struct version
	uint8 mode;            // boot loader mode
	uint8 current_rom;     // currently selected rom
	uint8 gpio_rom;        // rom to use for gpio boot
	uint8 count;           // number of roms in use
	uint8 unused[2];       // padding
	uint32 roms[MAX_ROMS]; // flash addresses of the roms
#ifdef BOOT_CONFIG_CHKSUM
	uint8 chksum;          // boot config chksum
#endif
} rboot_config;

Write a config structure as above to address 0x1000 on the flash. If you want
more than 4 roms (default) just increase MAX_ROMS when you compile rBoot.
Think about how you intend to layout your flash before you start!

  - magic should have value 0xe1 (defined as BOOT_CONFIG_MAGIC).
  - version is used in case the config structure changes in future, at the
    moment it is 0x01 (BOOT_CONFIG_VERSION).
  - mode can be 0x00 (MODE_STANDARD) or 0x01 (MODE_GPIO_ROM). If you set GPIO
    you will need to set gpio_rom as well. The sample GPIO code uses GPIO 16 on
    a nodemcu dev board, if you want to use a different GPIO you'll need to
    adapt the code in rBoot slightly.
  - current_rom is the rom to boot, numbered 0 to count-1.
  - gpio_rom is the rom to boot when the GPIO is triggered at boot.
  - count is the number of roms available (may be less than MAX_ROMS, but not
    more).
  - unused[2] is padding so the uint32 rom addresses are 4 bytes aligned.
  - roms is the array of flash address for the roms. The default generated
    config will contain two entries: 0x00002000 and 0x00082000.
  - chksum (if enabled, not by deafult) should be the xor of 0xef followed by
    each of the bytes of the config structure up to (but obviously not
    including) the chksum byte itself.

Linking user code
-----------------
Each rom will need to be linked with an appropriate linker file, specifying
where it will reside on the flash. If you are only flashing one rom to multiple
places on the flash it must be linked multiple times to produce the set of rom
images. This is the same as with the SDK loader.

Because there are endless possibilities for layout with this loader I don't
supply sample linker files. Instead I'll tell you how to make them.

For each rom slot on the flash take a copy of the eagle.app.v6.ld linker script
from the sdk. You then need to modify just one line in it for each rom:
  irom0_0_seg :                         org = 0x40240000, len = 0x3C000

Change the org address to be 0x40200000 (base memory mapped location of the
flash) + flash address + 0x10 (offset of data after the header).
So the logical place for your first rom is the third sector, address 0x2000.
  0x40200000 + 0x2000 + 0x10 = 0x40202010
If you use the default generated config the loader will expect to find the
second rom at flash address 0x82000, so the irom0_0_seg should be:
  0x40200000 + 0x82000 + 0x10 = 0x40282010
Ideally you would also adjust the len to help detect over sized sections at
link time, but more important is the overall size of the rom which you need to
ensure fits in the space you have allocated for it in your flash layout plan.

Then simply compile and link as you would normally for OTA updates with the SDK
boot loader, except using the linker scripts you've just prepared rather than
the ones supplied with the SDK. Remember when building roms to create them as
'new' type roms (for use with SDK boot loader v1.2+). Or if using my esptool2
use the -boot2 option. Note: the test loads included with rBoot are built with
-boot0 because they do not contain a .irom0.text section (and so the value of
irom0_0_seg in the linker file is irrelevant to them) but 'normal' user apps
always do.
