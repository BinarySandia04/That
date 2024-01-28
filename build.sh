#!/bin/bash
sudo rm -R .build 2>/dev/null
mkdir .build 2>/dev/null

cd .build
cmake ../
cmake --build . -j 16
cd ..
# cp build/src/wyrm/Wyrm scripts/Wyrm/

# Copiar tot allo a build
# cp -RL src/libs/* scripts/Wyrm/libs/src 2> /dev/null

# ../Wyrm/Wyrm
# ../Wyrm/Wyrm init
