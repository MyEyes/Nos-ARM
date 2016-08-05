CC = arm-none-eabi-gcc

#QEMU BUILD RULES
PI_QEMU_PLATFORM = /platform/qemu-pi/
PI_QEMU_ORIGIN=0x10000

PI_QEMU_BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(PI_QEMU_ORIGIN)
PI_QEMU_CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"..$(PI_QEMU_PLATFORM)platform.h\" -c -O2 -Wall -Wextra
PI_QEMU_LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(PI_QEMU_ORIGIN)

#General rules for files
BOOTFILE = boot/boot.S
SRC_FILES = $(wildcard kernel/*.c)
LD_FILE = boot/linker.ld


pi-qemu-all: pi-qemu-boot pi-qemu-cc pi-qemu-ld

pi-qemu-boot: $(PI_BOOTFILE)
		$(CC) $(PI_QEMU_BOOTFLAGS) $(BOOTFILE)

pi-qemu-cc: 	$(SRC_FILES)
		$(CC) $(PI_QEMU_CFLAGS) $(SRC_FILES)

pi-qemu-ld:	
		$(CC) -T $(LD_FILE) -o nos.elf  $(PI_QEMU_LDFLAGS) *.o
		
clean: 
		rm -f *o nos.elf nos.bin
		
install: nos.elf
		arm-none-eabi-objcopy nos.elf -O binary nos.bin