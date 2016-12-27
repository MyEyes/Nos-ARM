#!/bin/sh
mkdir -p lib
make -B std-all
cd usr/init
make -B
make install
cd ../..
make -B pi-qemu-all
make install
