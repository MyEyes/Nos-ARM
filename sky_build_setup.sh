#!/bin/sh
mkdir -p lib
make std-all
cd usr/init
make
make install
cd ../..
make -B pi-qemu-all
make install