PI_QEMU_PLATFORM = /platform/qemu-pi/
ORIGIN=0x10000

BOOTFLAGS = -mcpu=arm1176jzf-s -fpic -ffreestanding -c -Xassembler --defsym -Xassembler orig=$(ORIGIN)
CFLAGS = -mcpu=arm1176jzf-s -fpic -std=gnu99 -ffreestanding -D__PLATFORM__=\"..$(PI_QEMU_PLATFORM)platform.h\" -c -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib -Xlinker --defsym=orig=$(ORIGIN)
BOOTFILE = boot/boot.S