#!/bin/bash
# sudo rm -R .build 2>/dev/null
# sudo rm -R /usr/local/include/ZagIR/
sudo rm -R /usr/local/lib/zag

sudo mkdir /usr/local/lib/zag 2>/dev/null
sudo mkdir /usr/local/lib/zag/packages 2>/dev/null
sudo cp -R Std /usr/local/lib/zag/packages

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
