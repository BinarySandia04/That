@echo off
cd build
cmake ../
cmake --build .
cd ..

echo --------------------------------------------------------------------------------
cd build/Debug/
Glass.exe
cd ../..