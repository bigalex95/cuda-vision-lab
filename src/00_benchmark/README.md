# 00_benchmark - CPU vs GPU Performance Testing

This module provides a comprehensive benchmarking suite for comparing CPU and GPU implementations of fundamental array operations. It serves as a foundation for understanding performance characteristics and verifying correctness across the compute pipeline.

## Features

### Array Operations

- **Element-wise Addition**: Parallel addition of two arrays
- **Element-wise Multiplication**: Parallel multiplication of two arrays
- **Scalar Multiplication**: Broadcasting scalar across array
- **Dot Product**: Parallel reduction with shared memory optimization
- **Array Normalization**: Magnitude computation + element-wise division

### Benchmarking Infrastructure

- Automated CPU timing using high-resolution chrono
- GPU timing using CUDA events for accurate kernel measurements
- Multiple array sizes: 1M, 10M, 50M elements
- Warmup iterations to eliminate cold-start effects
- Result verification with configurable tolerance
- Speedup calculation and color-coded reporting

## Implementation Details

### CPU Implementation (`array_ops_cpu.cpp`)

- Sequential loop-based operations
- Cache-friendly access patterns
- Standard C++ math library

### GPU Implementation (`array_ops_gpu.cu`)

- CUDA kernels with 256 threads per block
- Optimized grid stride loops
- Shared memory reductions for dot product and normalization
- Two-stage reduction pattern for aggregate operations

### Benchmark Harness (`main.cpp`)

- Automatic GPU detection and capability reporting
- Random data initialization for realistic testing
- Iterative averaging (100 runs) for statistical stability
- Color-coded terminal output with verification status

## Build & Run

```bash
# From project root
mkdir -p build && cd build
cmake ..
make benchmark

# Run the benchmark suite
./bin/benchmark
```

## Expected Results

Typical speedup factors on modern GPUs:

- **Simple operations** (add, multiply, scale): 50-100x
- **Reduction operations** (dot product): 30-80x
- **Complex operations** (normalization): 20-50x

Performance scales with array size, with larger arrays showing better GPU utilization.

## Usage as Template

This module serves as a reference for:

1. Structuring CPU/GPU paired implementations
2. Integrating with the common benchmark utilities
3. Building verification into performance tests
4. Organizing CUDA projects with CMake

Copy this pattern when adding new algorithms to the vision pipeline.
