#!/bin/bash
rm -R build
cmake -B build -DCMAKE_MAKE_PROGRAM=mingw32-make -G "MinGW Makefiles" .
ls .
make 