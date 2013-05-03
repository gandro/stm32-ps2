#########################################
FIRMWARE = src/firmware.a
#########################################

# root directory
export ROOT=$(shell pwd)

# compiler toolchain
export CC=arm-none-eabi-gcc
export LD=arm-none-eabi-gcc
export AR=arm-none-eabi-ar
export AS=arm-none-eabi-as
export OBJCOPY=arm-none-eabi-objcopy
export GDB=arm-none-eabi-gdb

# default flags
export ASFLAGS=-g
export LDFLAGS=-L$(ROOT)/lib
export CFLAGS=-O1 -c -fno-common -mcpu=cortex-m3 -mthumb

# flash utils
export STFLASH=st-flash
export STUTIL=st-util

# libtinyprintf
LDFLAGS += -L$(ROOT)/lib/tinyprintf
CFLAGS += -I$(ROOT)/lib/tinyprintf

# libstm32f10x
LDFLAGS += -L$(ROOT)/lib/stm32f10x
CFLAGS += -I$(ROOT)/lib/stm32f10x
CFLAGS += -I$(ROOT)/lib/stm32f10x/inc/core 
CFLAGS += -I$(ROOT)/lib/stm32f10x/inc/peripherals
CFLAGS += -DSTM32F10X_MD_VL=1 -DUSE_STDPERIPH_DRIVER=1

# linker script and startup code
STARTUP = $(ROOT)/lib/stm32f10x/startup_stm32f10x_md_vl.s
LDFLAGS += -T$(ROOT)/lib/stm32f10x/stm32f100.ld

# additional compiler flags
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Wshadow -Wcast-qual

.PHONY: flash gdb-server gdb-client clean

all: firmware.bin

# main blob

firmware:
	$(MAKE) -C src

firmware.elf: firmware libstm32f10x libtinyprintf
	$(LD) $(LDFLAGS) -o $@ $(STARTUP) $(FIRMWARE) -lstm32f10x -ltinyprintf

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

# build libraries

libstm32f10x:
	$(MAKE) -C lib/stm32f10x libstm32f10x.a

libtinyprintf:
	$(MAKE) -C lib/tinyprintf libtinyprintf.a

# helper targets

flash: firmware.bin
	$(STFLASH) write firmware.bin 0x8000000

gdb-server:
	$(STUTIL) --stlinkv1

gdb-client: lib proj
	$(GDB) --eval-command="target extended-remote :4242" firmware.elf

clean:
	$(MAKE) -C lib/stm32f10x clean
	$(MAKE) -C lib/tinyprintf clean
	$(MAKE) -C src clean
	rm -f firmware.elf firmware.bin
