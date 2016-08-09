CC = arm-none-eabi-gcc

KERNEL_BASE = 0xc0000000

#QEMU BUILD RULES
PI_QEMU_PLATFORM = platform/qemu-pi/
PI_QEMU_ORIGIN=0x10000

PI_QEMU_BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(PI_QEMU_ORIGIN)
PI_QEMU_CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"$(PI_QEMU_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_QEMU_ORIGIN) -c -O2 -Wall -Wextra
PI_QEMU_LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(PI_QEMU_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)

#PI1-B BUILD RULES
PI_B_PLATFORM = platform/pi-modelb/
PI_B_ORIGIN=0x8000

PI_B_BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(PI_B_ORIGIN)
PI_B_CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"$(PI_B_PLATFORM)platform.h\" -D__ORIGIN__=$(PI_B_ORIGIN) -c -O2 -Wall -Wextra
PI_B_LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(PI_B_ORIGIN) -Xlinker --defsym=kernel_base=$(KERNEL_BASE)

#General rules for files
BOOTFILE = boot/boot.S
SRC_FILES = $(shell find kernel -name "*.c")
LD_FILE = boot/linker.ld
INCLUDE_PATH = -Iinclude/

#QEMU raspbi targets

pi-qemu-all: pi-qemu-boot pi-qemu-cc pi-qemu-ld

pi-qemu-boot: $(PI_BOOTFILE)
		$(CC) $(PI_QEMU_BOOTFLAGS) $(BOOTFILE)

pi-qemu-cc: 	$(SRC_FILES)
		$(CC) $(INCLUDE_PATH) $(PI_QEMU_CFLAGS) $(SRC_FILES)

pi-qemu-ld:	
		$(CC) -T $(LD_FILE) -o nos.elf  $(PI_QEMU_LDFLAGS) *.o
		
#raspbi modelb targets

pi-b-all: pi-b-boot pi-b-cc pi-b-ld

pi-b-boot: $(PI_BOOTFILE)
		$(CC) $(PI_B_BOOTFLAGS) $(BOOTFILE)

pi-b-cc: 	$(SRC_FILES)
		$(CC) $(INCLUDE_PATH) $(PI_B_CFLAGS) $(SRC_FILES)

pi-b-ld:	
		$(CC) -T $(LD_FILE) -o nos.elf  $(PI_B_LDFLAGS) *.o
		
#General targets
		
clean: 
		rm -f *o nos.elf nos.bin
		
install: nos.elf
		arm-none-eabi-objcopy nos.elf -O binary nos.bin