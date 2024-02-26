#!/bin/bash

cd .build
mkdir .build 2>/dev/null
cmake ../
cmake --build . -j 16
cd ..
cp .build/*.so .
