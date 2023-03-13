#!/bin/bash
cd ../../
./build.sh
cd tests/primes/
gcc primes.c -Wall -ansi -lm -o c-primes.exe
time python3 primes.py
time ../../build/src/That -a primes.that
time ./c-primes.exe