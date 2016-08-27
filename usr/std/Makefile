CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar

PI_B_GCCFLAGS = -mcpu=arm1176jzf-s -fpic -fpie -pie -std=gnu99 -ffreestanding -nostdlib -O2 -Wall -Wextra -Werror -I include/std
PI_B_CFLAGS = $(PI_B_GCCFLAGS) -c -D__PLATFORM__=\"platform/arm-common.h\"
PI_B_LDFLAGS = $(PI_B_GCCFLAGS)

#General rules for files
SRC_FILES = $(shell find usr/std -name "*.c")
OBJ_FILES = $(patsubst %.c,.obj/%.o,$(SRC_FILES))

INCLUDE_PATH = -Iinclude/

#raspbi modelb targets

all: all-ld

.obj/%.o: %.c
		mkdir -p $(@D)
		$(CC) $(INCLUDE_PATH) $(PI_B_CFLAGS) $< -o $@



all-ld: $(OBJ_FILES)
		$(AR) -rcs lib/libstd.a $^