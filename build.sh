#!/bin/bash
cmake -DCMAKE_C_FLAGS="-Werror" -B build .
cd build
make 