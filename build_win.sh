#!/bin/bash
cmake -B build -DCMAKE_MAKE_PROGRAM=mingw32-make -G "MinGW Makefiles" .
cd build
make 