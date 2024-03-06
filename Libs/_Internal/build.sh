#!/bin/bash
cd "$(dirname "$0")"

cd .build
mkdir .build 2>/dev/null
cmake ../
cmake --build . -j 
cd ..

sudo mkdir -p /usr/local/lib/zag/packages/_Internal 2>/dev/null

sudo cp .build/lib_internal.so /usr/local/lib/zag/packages/_Internal/.
sudo cp package.toml /usr/local/lib/zag/packages/_Internal/.
sudo cp -r src /usr/local/lib/zag/packages/_Internal/.
