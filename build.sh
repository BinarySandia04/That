#!/bin/bash
sudo mkdir /usr/local/lib/that 2>/dev/null
sudo mkdir /usr/local/lib/that/packages 2>/dev/null
sudo mkdir /usr/local/lib/that/binaries 2>/dev/null

Libs/_Internal/build.sh
Libs/StdIo/build.sh
Libs/StdMath/build.sh

mkdir .build 2>/dev/null

cd .build
cmake ../
cmake --build . -j
cd ..
