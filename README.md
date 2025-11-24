# CuVO-Kernels: High-Performance CUDA Visual Odometry

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![CUDA](https://img.shields.io/badge/CUDA-12.x-green.svg)

**CuVO-Kernels** is a performance-oriented study of standard Computer Vision algorithms implemented from scratch in CUDA C++. The project aims to benchmark and optimize the "Frontend" of a SLAM system (Feature Extraction, Matching, and Tracking) by leveraging GPU hardware architecture.

## 🚀 Project Roadmap & Benchmarks

| Phase | Algorithm | CUDA Optimization Technique | Speedup (vs CPU) |
|-------|-----------|-----------------------------|------------------|
| **01** | Grayscale & Thresholding | Global Memory Coalescing | *Pending* |
| **02** | Gaussian Blur | Shared Memory Tiling & Halo Handling | *Pending* |
| **03** | Histogram Calculation | Atomic Operations & Local Reductions | *Pending* |
| **04** | Harris Corner Detector | CUDA Streams & Asynchronous Execution | *Pending* |
| **05** | BRIEF/ORB Matcher | Warp Intrinsics (`__popc`, `__shfl`) | *Pending* |
| **06** | Visual Odometry Pipeline | End-to-End Frame Tracking | *Pending* |

## 🛠️ Architecture

* **src/**: Contains isolated modules for each algorithmic stage.
* **common/**: Shared utilities for precise benchmarking.
* **data/**: Placeholders for the KITTI Odometry Dataset.

## 💻 Building the Project

**Requirements:**
* Ubuntu 22.04 LTS
* CUDA Toolkit 12.x
* OpenCV 4.x
* CMake 3.18+

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 👤 Author
**Ali (bigalex95)** - Computer Vision & Machine Learning Engineer
