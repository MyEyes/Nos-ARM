CC = arm-none-eabi-gcc

KERNEL_BASE = 0xc0000000

#ARM11 rules
ARM11_ARCH_FILES = $(shell find arch/ARM11/ -name "*.c")
ARM11_BOOTFILE = arch/ARM11/boot/boot.S
ARM11_LDFILE = arch/ARM11/boot/linker.ld

#QEMU BUILD RULES
PI_QEMU_PLATFORM = platform/qemu-pi/
PI_QEMU_ORIGIN=0x10000

PI_QEMU_BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(PI_QEMU_ORIGIN)
PI_QEMU_CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"$(PI_QEMU_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_QEMU_ORIGIN) -c -O2 -Wall -Wextra
PI_QEMU_LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(PI_QEMU_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)
PI_QEMU_ARCH_FILES = $(ARM11_ARCH_FILES)
PI_QEMU_BOOTFILE = $(ARM11_BOOTFILE)
PI_QEMU_LDFILE = $(ARM11_LDFILE)

#PI1-B BUILD RULES
PI_B_PLATFORM = platform/pi-modelb/
PI_B_ORIGIN=0x8000

PI_B_BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(PI_B_ORIGIN)
PI_B_CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"$(PI_B_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_B_ORIGIN) -c -O2 -Wall -Wextra
PI_B_LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(PI_B_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)
PI_B_ARCH_FILES = $(ARM11_ARCH_FILES)
PI_B_BOOTFILE = $(ARM11_BOOTFILE)
PI_B_LDFILE = $(ARM11_LDFILE)

#General rules for files
SRC_FILES = $(shell find kernel -name "*.c")
INCLUDE_PATH = -Iinclude/

#QEMU raspbi targets

pi-qemu-all: pi-qemu-boot pi-qemu-cc pi-qemu-ld

pi-qemu-boot: $(PI_BOOTFILE)
		$(CC) $(PI_QEMU_BOOTFLAGS) $(PI_QEMU_BOOTFILE)

pi-qemu-cc: 	$(SRC_FILES)
		$(CC) $(INCLUDE_PATH) $(PI_QEMU_CFLAGS) $(SRC_FILES) $(PI_QEMU_ARCH_FILES)

pi-qemu-ld:	
		$(CC) -T $(PI_QEMU_LDFILE) -o nos.elf  $(PI_QEMU_LDFLAGS) *.o
		
#raspbi modelb targets

pi-b-all: pi-b-boot pi-b-cc pi-b-ld

pi-b-boot: $(PI_BOOTFILE)
		$(CC) $(PI_B_BOOTFLAGS) $(PI_B_BOOTFILE)

pi-b-cc: 	$(SRC_FILES)
		$(CC) $(INCLUDE_PATH) $(PI_B_CFLAGS) $(SRC_FILES) $(PI_B_ARCH_FILES)

pi-b-ld:	
		$(CC) -T $(PI_B_LDFILE) -o nos.elf  $(PI_B_LDFLAGS) *.o
		
#General targets
		
clean: 
		rm -f *o nos.elf nos.bin
		
install: nos.elf
		arm-none-eabi-objcopy nos.elf -O binary nos.bin
