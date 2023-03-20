#!/bin/bash
cd ../../
./build.sh
cd tests/bigfor/
gcc fors.cpp -Wall -ansi -lm -o c-fors.exe
echo "--- Python3 ---"
time python3 fors.py
echo "--- That ---"
time ../../build/src/That -a fors.that
echo "--- C++ ---"
time ./c-fors.exe