#!/bin/bash

cd `dirname $0`
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain/TC-68k-nano.cmake ..
