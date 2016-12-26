CC = arm-none-eabi-gcc

KERNEL_BASE = 0xc0000000

#ARM11 rules
ARM11_ARCH_FILES = $(shell find arch/ARM11/ -name "*.c")
ARM11_BOOTFILE = arch/ARM11/boot/boot.S
ARM11_LDFILE = arch/ARM11/boot/linker.ld
ARM11_OBJ_FILES = $(patsubst %.c,.obj/%.o,$(ARM11_ARCH_FILES))

#QEMU BUILD RULES
PI_QEMU_PLATFORM = platform/qemu-pi/
PI_QEMU_ORIGIN=0x10000

PI_QEMU_GCCFLAGS = -O2 -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -nostdlib -Wall -Wextra -Werror -I include/std
PI_QEMU_BOOTFLAGS = $(PI_QEMU_GCCFLAGS) -c -Xassembler --defsym -Xassembler orig=$(PI_QEMU_ORIGIN)
PI_QEMU_CFLAGS =  $(PI_QEMU_GCCFLAGS) -D__PLATFORM__=\"$(PI_QEMU_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_QEMU_ORIGIN) -c 
PI_QEMU_LDFLAGS =  $(PI_QEMU_GCCFLAGS) -Xlinker --defsym=orig=$(PI_QEMU_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)
PI_QEMU_ARCH_FILES = $(ARM11_ARCH_FILES)
PI_QEMU_BOOTFILE = $(ARM11_BOOTFILE)
PI_QEMU_LDFILE = $(ARM11_LDFILE)

#General rules for files
SRC_FILES = $(shell find kernel -name "*.c")
OBJ_FILES = $(patsubst %.c,.obj/%.o,$(SRC_FILES))

INCLUDE_PATH = -Iinclude/

#QEMU raspbi targets

.obj/%.o: %.c
		mkdir -p $(@D)
		$(CC) $(INCLUDE_PATH) $(PI_QEMU_CFLAGS) $< -o $@
		
pi-qemu-all: pi-qemu-boot pi-qemu-cc pi-qemu-ld

pi-qemu-boot: $(PI_BOOTFILE)
		$(CC) $(PI_QEMU_BOOTFLAGS) $(PI_QEMU_BOOTFILE)

pi-qemu-cc: $(SRC_FILES) $(PI_QEMU_ARCH_FILES)

pi-qemu-ld:	$(OBJ_FILES) $(ARM11_OBJ_FILES) boot.o
		$(CC) -T $(PI_QEMU_LDFILE) -o nos.elf  $(PI_QEMU_LDFLAGS) $^ -lgcc
		
