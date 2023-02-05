#!/bin/bash
mkdir build
cd build
cmake ../
cmake --build .
cd ..

echo "--------------------------------------------------------------------------------"
build/src/Radic