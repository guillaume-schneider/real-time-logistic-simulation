@echo off

:: Check if the build directory exists
if not exist "build" (
    mkdir build
)

:: Change to the build directory
cd build

:: Run CMake to configure and build
cmake ..
cmake --build .
