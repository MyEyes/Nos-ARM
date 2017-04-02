#!/bin/sh
set -e
mkdir -p lib
make -B std-all
cd usr/init
make -B all PLATFORM=bcm2385
make install
cd ../..
cd tools/devmap_bld
./devmap_bld devmap_pi.txt devmap_pi-b
cp devmap_pi-b ../../hw_map.map
cd ../..
make -B pi-b-all
make install
