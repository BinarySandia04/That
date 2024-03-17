#!/bin/bash
# sudo rm -R .build 2>/dev/null
# sudo rm -R /usr/local/include/ZagIR/
# ./build-vendors.sh
sudo mkdir /usr/local/lib/zag 2>/dev/null
sudo mkdir /usr/local/lib/zag/packages 2>/dev/null
sudo mkdir /usr/local/lib/zag/binaries 2>/dev/null

Libs/_Internal/build.sh
Libs/Std/build.sh

mkdir .build 2>/dev/null

cd .build
cmake ../
cmake --build . -j
cd ..
# cp build/src/wyrm/Wyrm scripts/Wyrm/

# Copiar tot allo a build
# cp -RL src/libs/* scripts/Wyrm/libs/src 2> /dev/null

# ../Wyrm/Wyrm
# ../Wyrm/Wyrm init
