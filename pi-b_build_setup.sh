#!/bin/sh
set -e
mkdir -p lib
make std-all
cd usr/init
make
make install
cd ../..
make -B pi-b-all
make install
