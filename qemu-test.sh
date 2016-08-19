#!/bin/bash
QEMU_PATH=$(printf "%q" "/cygdrive/c/Program Files/qemu/qemu-system-arm")
if [\"$(uname)\" == \"Linux\"]
	then
		qemu-system-arm -D qemu.log -kernel nos.elf -cpu arm1176 -m 256 -M raspi2 -d int -nographic
else
	"/cygdrive/c/Program Files/qemu/qemu-system-arm" -D qemu.log -kernel nos.elf -cpu arm1176 -m 256 -M raspi -d int
fi
