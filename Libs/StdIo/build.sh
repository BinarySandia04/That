#!/bin/bash
cd "$(dirname "$0")"

mkdir .build 2>/dev/null
cd .build
cmake ../
cmake --build . -j 
cd ..

# sudo mkdir -p /usr/local/lib/zag/packages/Std 2>/dev/null
# sudo cp .build/libstd.so /usr/local/lib/zag/packages/Std/.
# sudo cp package.toml /usr/local/lib/zag/packages/Std/.
# sudo cp -r src /usr/local/lib/zag/packages/Std/.

mkdir -p ~/.zag/sources/StdIo
cp -r * ~/.zag/sources/StdIo/.
