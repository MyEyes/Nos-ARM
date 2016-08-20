CC = arm-none-eabi-gcc

ORIGIN=0x100000

GCCFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -ffreestanding -nostdlib -O2 -Wall -Wextra
LDFLAGS = $(GCCFLAGS) -Xlinker --defsym=orig=$(ORIGIN)


test: 
		$(CC) $(GCCFLAGS) -c usr_init.c -o usr_init.o  -lgcc
		$(CC) -T linker.ld $(LDFLAGS) usr_init.o -o usr_init  -lgcc
		arm-none-eabi-objcopy --set-section-flags .bss=alloc,load,contents usr_init -O binary payload