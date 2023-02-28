#!/bin/bash
mkdir build
cd build
cmake ../
cmake --build .
cd ..

build/src/That -a -d examples/test.that