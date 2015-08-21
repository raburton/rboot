#
# Makefile for rBoot
# https://github.com/raburton/esp8266
#

ESPTOOL2 ?= ../esptool2/esptool2

# XTENSA_BINDIR needs trailing slash or can be blank if already in $PATH
XTENSA_BINDIR ?= C:/xtensa-lx106-elf/bin/
CC := $(addprefix $(XTENSA_BINDIR),xtensa-lx106-elf-gcc)
LD := $(addprefix $(XTENSA_BINDIR),xtensa-lx106-elf-gcc)

CFLAGS    = -Os -O3 -Wpointer-arith -Wundef -Werror -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals  -D__ets__ -DICACHE_FLASH
LDFLAGS   = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
LD_SCRIPT = eagle.app.v6.ld

BUILD_DIR = build
FIRMW_DIR = firmware

.SECONDARY:

all: $(BUILD_DIR) $(FIRMW_DIR) $(FIRMW_DIR)/rboot.bin $(FIRMW_DIR)/testload1.bin $(FIRMW_DIR)/testload2.bin

$(BUILD_DIR):
	@mkdir -p $@

$(FIRMW_DIR):
	@mkdir -p $@

$(BUILD_DIR)/rboot-stage2a.o: rboot-stage2a.c rboot-private.h rboot.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@
	
$(BUILD_DIR)/rboot-stage2a.elf: $(BUILD_DIR)/rboot-stage2a.o
	@echo "LD $@"
	@$(LD) -Trboot-stage2a.ld $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

$(BUILD_DIR)/rboot-hex2a.h: $(BUILD_DIR)/rboot-stage2a.elf
	@echo "FW $@"
	@$(ESPTOOL2) -quiet -header $< $@ .text

$(BUILD_DIR)/rboot.o: rboot.c rboot-private.h rboot.h $(BUILD_DIR)/rboot-hex2a.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.c %.h
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o
	@echo "LD $@"
	@$(LD) -T$(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

$(FIRMW_DIR)/%.bin: $(BUILD_DIR)/%.elf
	@echo "FW $@"
	@$(ESPTOOL2) -quiet -bin -boot0 $< $@ .text .rodata

clean:
	@echo "RM $(BUILD_DIR) $(FIRMW_DIR)"
	@rm -rf $(BUILD_DIR)
	@rm -rf $(FIRMW_DIR)
