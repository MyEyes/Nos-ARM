#!/bin/bash
QEMU_PATH=$(printf "%q" "/cygdrive/c/Program Files/qemu/qemu-system-arm")
if test $(uname) == Linux
	then
		qemu-system-arm -D qemu.log -kernel nos.img -cpu arm1176 -m 512 -M raspi2 -d int -nographic
else
	"/cygdrive/c/Program Files/qemu/qemu-system-arm" -D qemu.log -kernel nos.img -cpu arm1176 -m 512 -M raspi -d int,guest_errors -serial file:test.txt
fi
