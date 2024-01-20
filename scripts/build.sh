#!/bin/bash
cd ..
sudo rm -R build
mkdir build
cd build
cmake ../
cmake --build . -j 8
cd ..
# cp build/src/wyrm/Wyrm scripts/Wyrm/

# Copiar tot allo a build
# cp -RL src/libs/* scripts/Wyrm/libs/src 2> /dev/null

# ../Wyrm/Wyrm
# ../Wyrm/Wyrm init
