CC = arm-none-eabi-gcc

ORIGIN=0x100000

MODFILES = $(shell ./get_platform_dirs.sh $(PLATFORM))
PLATFORM_INCLUDE = plat/$(PLATFORM)/platform.h

BOOTFS_FILE = bootfs/bootfs.img
BOOTFS_TARGET = bootfs.o

OBJ_FILES = $(patsubst %.c,.obj/%.o,$(MODFILES)) $(BOOTFS_TARGET)

MODFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -DPLATFORM_INCLUDE=\"$(PLATFORM_INCLUDE)\" -c -I../../include/std -ffreestanding -nostdlib -O2 -Wall -Wextra
GCCFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -DPLATFORM_INCLUDE=\"$(PLATFORM_INCLUDE)\" -I../../include/std -ffreestanding -nostdlib -O2 -Wall -Wextra
LDFLAGS = $(GCCFLAGS) -Xlinker --defsym=orig=$(ORIGIN)

.obj/%.o: %.c
		mkdir -p $(@D)
		$(CC) $(MODFLAGS) $< -o $@

bootfs: $(BOOTFS_FILE)
	arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm $(BOOTFS_FILE) $(BOOTFS_TARGET)

all: $(OBJ_FILES) bootfs
		$(CC) $(GCCFLAGS) -c usr_init.c startup.c -lgcc -L../../lib
		$(CC) -T linker.ld $(LDFLAGS) startup.o usr_init.o $(OBJ_FILES) -o usr_init  -lgcc -L../../lib -lstd
		arm-none-eabi-objcopy --set-section-flags .bss=alloc,load,contents usr_init -O binary payload
	
install:
		cp payload ../../
