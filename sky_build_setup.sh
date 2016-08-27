#!/bin/sh
make std-all
cd usr/init
make
make install
cd ../..
make -B
make install