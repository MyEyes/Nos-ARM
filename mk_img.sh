#!/bin/sh

ALIGN=4096
FILESIZE=$(stat -c%s nos.bin)
echo "$FILESIZE"

SECTIONS=$((($FILESIZE+4095)/4096))
END=$(($SECTIONS*$ALIGN-1))
echo "$SECTIONS"
cat nos.bin > nos.img
dd if=/dev/zero of=nos.img seek=$END bs=1 count=1 conv=notrunc,noerror
cat hw_map.map >> nos.img
cat payload >> nos.img