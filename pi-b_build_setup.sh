#!/bin/sh
set -e
mkdir -p lib
make -B std-all
cd usr/init
make -B
make install
cd ../..
make -B pi-b-all
make install
