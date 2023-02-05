@echo off
mkdir build
cd build
cmake ../
cmake --build .
cd ..

echo --------------------------------------------------------------------------------
cd build/src/Debug/
Radic.exe
cd ../../..