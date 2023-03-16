#!/bin/bash
./build.sh
valgrind --leak-check=full ./That -a test.that