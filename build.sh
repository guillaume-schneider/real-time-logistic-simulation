#!/bin/bash

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Move into the build directory
cd build

# Run CMake to configure and build
cmake ..
cmake --build .
