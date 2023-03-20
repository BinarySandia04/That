#!/bin/bash
cd ../../
./build.sh
cd tests/primes/
g++ primes.cpp -Wall -ansi -lm -o c-primes.exe
time ../../build/src/That -a primes.that
time python3 primes.py
time ./c-primes.exe