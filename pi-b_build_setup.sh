#!/bin/sh
set -e
mkdir -p lib
make -B std-all
cd usr/init
make -B all PLATFORM=bcm2385
make install
cd ../..
make -B pi-b-all
make install
