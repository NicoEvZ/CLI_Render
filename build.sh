#!/bin/bash
cd src
rm -R build
cmake -B build .
cd build
make 