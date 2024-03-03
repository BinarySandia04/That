#!/bin/bash
cd "$(dirname "$0")"

cd .build
mkdir .build 2>/dev/null
cmake ../
cmake --build . -j 
cd ..

sudo mkdir -p /usr/local/lib/zag/packages/Std 2>/dev/null

sudo cp .build/libstd.so /usr/local/lib/zag/packages/Std/.
sudo cp package.toml /usr/local/lib/zag/packages/Std/.
