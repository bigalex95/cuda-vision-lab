#!/bin/bash

# ==========================================
# CUDA Vision Lab - Dependency Installer
# OS: Ubuntu 22.04 LTS
# Author: bigalex95
# ==========================================

set -e # Exit on error

echo "🚀 Starting Dependency Installation for CuVO-Kernels..."

# 1. Update Repositories
echo "📦 Updating apt repositories..."
sudo apt-get update

# 2. Install Build Tools (CMake, GCC, G++)
echo "🛠️ Installing Build Essentials & CMake..."
sudo apt-get install -y build-essential cmake git pkg-config

# 3. Install OpenCV (Development Headers)
# On Ubuntu 22.04, this installs OpenCV ~4.5.4
echo "👁️ Installing OpenCV 4 (libopencv-dev)..."
sudo apt-get install -y libopencv-dev

# 4. Install CUDA Toolkit (If not present)
# NOTE: This assumes you have NVIDIA Drivers installed.
# If nvcc is missing, we try to install the toolkit.
if ! command -v nvcc &> /dev/null
then
    echo "⚠️  CUDA Compiler (nvcc) not found. Installing nvidia-cuda-toolkit..."
    sudo apt-get install -y nvidia-cuda-toolkit
else
    echo "✅ CUDA Compiler (nvcc) is already installed."
fi

# 5. Verify Installation
echo "--------------------------------------------------"
echo "✅ Installation Complete."
echo "--------------------------------------------------"
echo "CMake Version: $(cmake --version | head -n1)"
echo "OpenCV Version: $(dpkg -l | grep libopencv-dev | awk '{print $3}')"
if command -v nvcc &> /dev/null; then
    echo "CUDA Version: $(nvcc --version | grep release | awk '{print $5,$6}')"
else
    echo "❌ CUDA (nvcc) is still missing. Please install NVIDIA drivers manually."
fi
echo "--------------------------------------------------"