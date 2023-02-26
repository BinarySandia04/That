#!/bin/bash
mkdir build
cd build
cmake ../
cmake --build .
cd ..

valgrind build/src/That examples/test.that