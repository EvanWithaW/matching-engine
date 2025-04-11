#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Configure with CMake
cmake ..

# Build the project in parallelized compile mode
make -j$(sysctl -n hw.ncpu)

# Return to the original directory
cd ..

echo "Build completed successfully!"
echo "Run the main program with: ./build/matching_engine"
echo "Run the tests with: ./build/tests/unit_tests"
