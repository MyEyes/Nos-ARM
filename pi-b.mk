CC = arm-none-eabi-gcc

KERNEL_BASE = 0xc0000000

#ARM11 rules
ARM11_ARCH_FILES = $(shell find arch/ARM11/ -name "*.c")
ARM11_BOOTFILE = arch/ARM11/boot/boot.S
ARM11_LDFILE = arch/ARM11/boot/linker.ld
ARM11_OBJ_FILES = $(patsubst %.c,.obj/%.o,$(ARM11_ARCH_FILES))

#PI1-B BUILD RULES
PI_B_PLATFORM = platform/pi-modelb/
PI_B_ORIGIN=0x8000

PI_B_GCCFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -ffreestanding -nostdlib -O2 -Wall -Wextra -Werror -I include/std
PI_B_BOOTFLAGS = $(PI_B_GCCFLAGS) -c -Xassembler --defsym -Xassembler orig=$(PI_B_ORIGIN)
PI_B_CFLAGS = $(PI_B_GCCFLAGS) -c -D__PLATFORM__=\"$(PI_B_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_B_ORIGIN)
PI_B_LDFLAGS = $(PI_B_GCCFLAGS) -Xlinker --defsym=orig=$(PI_B_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)
PI_B_ARCH_FILES = $(ARM11_ARCH_FILES)
PI_B_BOOTFILE = $(ARM11_BOOTFILE)
PI_B_LDFILE = $(ARM11_LDFILE)

#General rules for files
SRC_FILES = $(shell find kernel -name "*.c")
OBJ_FILES = $(patsubst %.c,.obj/%.o,$(SRC_FILES))

INCLUDE_PATH = -Iinclude/

#raspbi modelb targets

.obj/%.o: %.c
		mkdir -p $(@D)
		$(CC) $(INCLUDE_PATH) $(PI_B_CFLAGS) $< -o $@

pi-b-all: pi-b-boot pi-b-cc pi-b-ld

pi-b-boot: $(PI_BOOTFILE)
		$(CC) $(PI_B_BOOTFLAGS) $(PI_B_BOOTFILE)

pi-b-cc: 	$(SRC_FILES) $(ARM11_OBJ_FILES)

pi-b-ld: $(OBJ_FILES) $(ARM11_OBJ_FILES) boot.o
		$(CC) -T $(PI_B_LDFILE) $(PI_B_LDFLAGS) -o nos.elf  $^ -lgcc