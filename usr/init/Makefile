CC = arm-none-eabi-gcc

ORIGIN=0x100000

MODFILES = $(shell find mod -name "*.c")
OBJ_FILES = $(patsubst %.c,.obj/%.o,$(MODFILES))

MODFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -c -I../../include/std -ffreestanding -nostdlib -O2 -Wall -Wextra
GCCFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -I../../include/std -ffreestanding -nostdlib -O2 -Wall -Wextra
LDFLAGS = $(GCCFLAGS) -Xlinker --defsym=orig=$(ORIGIN)

.obj/%.o: %.c
		mkdir -p $(@D)
		$(CC) $(MODFLAGS) $< -o $@

test: $(OBJ_FILES)
		$(CC) $(GCCFLAGS) -c usr_init.c startup.c -lgcc -L../../lib
		$(CC) -T linker.ld $(LDFLAGS) startup.o usr_init.o $(OBJ_FILES) -o usr_init  -lgcc -L../../lib -lstd
		arm-none-eabi-objcopy --set-section-flags .bss=alloc,load,contents usr_init -O binary payload
		
install:
		cp payload ../../