# Use NVIDIA's official CUDA development image (Ubuntu 22.04 base)
FROM nvidia/cuda:12.2.0-devel-ubuntu22.04

# Metadata
LABEL maintainer="bigalex95 <alibek060395@gmail.com>"
LABEL description="CUDA Vision Lab Environment"

# Prevent interactive prompts during apt install
ENV DEBIAN_FRONTEND=noninteractive

# 1. Install System Dependencies
# We include libopencv-dev for the CPU baseline, but we won't use GTK/GUI features in Docker
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libopencv-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# 2. Set Working Directory
WORKDIR /app

# 3. Copy Source Code
COPY . /app

# 4. Build Instructions
# We build in release mode for accurate benchmarking
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# 5. Default Command (Runs the benchmark suite)
# Note: ensure your CMakeLists creates a target named 'run_benchmarks' or similar
# For now, we point to the Phase 1 benchmark as a default example
CMD ["./build/bin/01_pixel_ops_benchmark"]