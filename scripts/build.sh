#!/bin/bash

# ==========================================
# CUDA Vision Lab - Build Script
# OS: Ubuntu 22.04 LTS
# Author: bigalex95
# ==========================================

set -e # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "📁 Creating build directory..."
    mkdir -p build
fi

cd build

# Configure with CMake
echo "⚙️  Configuring CMake..."
cmake ..

# Build specific target or all
if [ -n "$1" ]; then
    echo "🔨 Building target: $1"
    make -j$(nproc) "$1"
else
    echo "🔨 Building all targets..."
    make -j$(nproc)
fi

echo "✅ Build complete!"