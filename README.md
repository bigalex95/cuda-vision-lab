# cuda-vision-lab

A work-in-progress learning lab for CUDA kernels aimed at the front end of a visual SLAM/VO pipeline (the CMake project is named CuVO-Kernels). **Only stage 00, a CPU vs GPU array-operation benchmark, is implemented.** Stages 01-06 are placeholder directories.

## Status

| Stage | Directory | Content | Status |
| --- | --- | --- | --- |
| 00 | `src/00_benchmark` | CPU vs GPU array ops (add, multiply, scale, dot product, normalize) with CUDA-event timing and correctness check | done |
| 01 | `src/01_pixel_ops` | Grayscale and thresholding | planned |
| 02 | `src/02_convolution` | Gaussian blur (shared-memory tiling) | planned |
| 03 | `src/03_histogram` | Histogram (atomics, local reductions) | planned |
| 04 | `src/04_features` | Harris corner detector (CUDA streams) | planned |
| 05 | `src/05_matching` | BRIEF/ORB matcher (warp intrinsics) | planned |
| 06 | `src/06_visual_odom` | Frame-to-frame visual odometry on KITTI | planned |

No computer-vision kernel exists yet, so nothing here is compared against OpenCV so far. Planned stages will be added only with measured results and correctness checks.

## Stage 00 results

Measured on an NVIDIA GeForce RTX 3060 Laptop GPU (compute capability 8.6, CUDA 12.6) with an AMD Ryzen 7 5800H CPU, single-threaded CPU reference in plain C++ (`-O3`), 100 iterations. GPU time is kernel-only via CUDA events on data already resident on the device (host/device transfers are not included). Single run; the speedups for the 1M-element case varied between runs (for example array addition 4.0x and 7.6x), so treat the numbers as indicative.

| Operation (50M floats) | CPU | GPU | Speedup |
| --- | --- | --- | --- |
| Array addition | 39.8 ms | 1.88 ms | 21.2x |
| Array multiplication | 26.1 ms | 1.88 ms | 13.9x |
| Scalar multiplication | 19.0 ms | 1.26 ms | 15.1x |
| Dot product | 38.2 ms | 3.01 ms | 12.7x |
| Normalization | 69.0 ms | 4.34 ms | 15.9x |

Speedups across sizes in the same run:

| Operation | 1M | 10M | 50M |
| --- | --- | --- | --- |
| Array addition | 7.6x | 14.0x | 21.2x |
| Array multiplication | 4.2x | 14.1x | 13.9x |
| Scalar multiplication | 3.2x | 16.5x | 15.1x |
| Dot product | 3.5x | 10.0x | 12.7x |
| Normalization | 4.6x | 14.6x | 15.9x |

These are memory-bound operations, so they say little about vision kernels; they validate the harness and the toolchain.

## Build and run

Prerequisites: CUDA toolkit (built here with 12.6), CMake 3.18+, OpenCV (found by CMake; used by the common utilities), an NVIDIA GPU. `scripts/install_deps.sh` installs dependencies on Ubuntu 22.04.

```bash
./scripts/build.sh benchmark     # cmake + make
./scripts/run.sh benchmark       # runs build/bin/benchmark
```

The default `CMAKE_CUDA_ARCHITECTURES` list is `52 60 61 70 75 80 86`; pass `-DCMAKE_CUDA_ARCHITECTURES=<arch>` for your GPU. On the machine above, `cuda_runtime.h` was not found by the C++ compiler until the CUDA include directory was added, so the numbers were produced with:

```bash
cmake -B build -DCMAKE_CUDA_ARCHITECTURES=86 -DCMAKE_CXX_FLAGS="-I/usr/local/cuda-12.6/include"
cmake --build build --target benchmark -j && ./build/bin/benchmark
```

Docker: `docker build -t cuda-vision-lab .` then `docker run --gpus all cuda-vision-lab` (not re-tested here).

## Project structure

```
common/        benchmark and visualization helpers (docs/COMMON_UTILS.md)
src/00_benchmark/   array-op benchmark (CPU + CUDA)
src/01..06_*/       empty placeholders for planned stages
scripts/       build.sh, run.sh, install_deps.sh
docs/          COMMON_UTILS.md
Dockerfile
```

## Author

Alibek Erkabayev (bigalex95). See [LICENSE](LICENSE).
