#!/bin/bash

# ==========================================
# CUDA Vision Lab - Run Script
# OS: Ubuntu 22.04 LTS
# Author: bigalex95
# ==========================================

set -e # Exit on error

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# Check if build directory exists
if [ ! -d "build/bin" ]; then
    echo "❌ Build directory not found. Please run ./scripts/build.sh first"
    exit 1
fi

# Run specific executable or default to benchmark
if [ -n "$1" ]; then
    EXECUTABLE="$1"
else
    EXECUTABLE="benchmark"
fi

if [ ! -f "build/bin/$EXECUTABLE" ]; then
    echo "❌ Executable 'build/bin/$EXECUTABLE' not found"
    echo "Available executables:"
    ls -1 build/bin/ 2>/dev/null || echo "  (none)"
    exit 1
fi

echo "🚀 Running: $EXECUTABLE"
echo ""
./build/bin/"$EXECUTABLE"