#!/bin/bash
g++ primes.cpp -Wall -ansi -lm -o c-primes.exe
./That -c primes.that
echo "--- That (I) ---"
time ./That primes.that
echo "--- That (C) ---"
time ./primes
echo "--- Python ---"
time python3 primes.py
echo "--- C++ ---"
time ./c-primes.exe