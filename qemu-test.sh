#!/bin/sh
QEMU_PATH=$(printf "%q" "/cygdrive/c/Program Files/qemu/qemu-system-arm")
"/cygdrive/c/Program Files/qemu/qemu-system-arm" -kernel nos.elf -cpu arm1176 -m 256 -M raspi