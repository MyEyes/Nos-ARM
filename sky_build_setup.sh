#!/bin/sh
mkdir -p lib
make -B std-all
cd usr/init
make -B all PLATFORM=bcm2385
make install
cd ../..
cd tools/devmap_bld
./devmap_bld devmap_pi2.txt devmap_qemu_pi
cp devmap_qemu_pi ../../hw_map.map
cd ../..
make -B pi-qemu-all
make install
