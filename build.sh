#!/bin/bash
rm -R build
cmake -B build .
cd build
make 