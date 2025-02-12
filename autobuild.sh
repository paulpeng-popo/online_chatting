#!/bin/bash

# -p: create parent directories as needed
mkdir -p build
# -S: source directory
# -B: build directory
cmake -S . -B build

# build the project
cd build && make && cd ..

# if the build is successful, move the executable outside
if [ -f build/server ]; then
    mv build/server .
fi

if [ -f build/client ]; then
    mv build/client .
fi

# clean up
rm -rf build
