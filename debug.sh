#!/bin/bash
sudo rm -R .build 2>/dev/null

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir .build 2>/dev/null

cd .build
cmake -DCMAKE_BUILD_TYPE=Debug ../
cmake --build . -j 8
cd ..

mv .build/src/zag/zag .build/Zag
