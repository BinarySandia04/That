#!/bin/bash
cd external/cppparser/CppParser
mkdir builds
cd builds
cmake ..
make && make test
