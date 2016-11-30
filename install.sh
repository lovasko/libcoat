#!/bin/sh

INC_DIR=/usr/include
LIB_DIR=/usr/lib

cp -v bin/libcoat.so "${LIB_DIR}"
cp -v src/coat.h "${INC_DIR}"
