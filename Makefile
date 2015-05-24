
CFLAGS = -Os -O3 -Wpointer-arith -Wundef -Werror -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals  -D__ets__ -DICACHE_FLASH
LDFLAGS = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
LD_SCRIPT = eagle.app.v6.ld

CC = xtensa-lx106-elf-gcc
LD = xtensa-lx106-elf-gcc

ESPTOOL2 = d:/Projects/esp8266/esptool2/Release/esptool2.exe

.SECONDARY:

all: rboot.bin testload1.bin testload2.bin

rboot-stage2a.o: rboot-stage2a.c rboot-private.h rboot.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@
	
rboot-stage2a.elf: rboot-stage2a.o
	@echo "LD $@"
	@$(LD) -Trboot-stage2a.ld $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

rboot-hex2a.h: rboot-stage2a.elf
	@echo "ESPTOOL2 $@"
	@$(ESPTOOL2) -quiet -header $< $@ .text

rboot.o: rboot.c rboot-private.h rboot.h rboot-hex2a.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c %.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

%.elf: %.o
	@echo "LD $@"
	@$(LD) -T$(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

%.bin: %.elf
	@echo "ESPTOOL2 $@"
	@$(ESPTOOL2) -quiet -bin -boot0 $< $@ .text .rodata

clean:
	@echo "RM *.bin *.elf *.o rboot-hex2a.h"
	@rm -f *.bin
	@rm -f *.elf
	@rm -f *.o
	@rm -f rboot-hex2a.h
