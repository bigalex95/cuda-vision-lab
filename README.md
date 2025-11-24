# CuVO-Kernels: High-Performance CUDA Visual Odometry

**CuVO-Kernels** is a performance-oriented study of standard Computer Vision algorithms implemented from scratch in CUDA C++. The project aims to benchmark and optimize the "Frontend" of a SLAM system (Feature Extraction, Matching, and Tracking) by leveraging GPU hardware architecture.

## 🚀 Project Roadmap & Benchmarks

|           |                          |                                       |                      |
| --------- | ------------------------ | ------------------------------------- | -------------------- |
| **Phase** | **Algorithm**            | **CUDA Optimization Technique**       | **Speedup (vs CPU)** |
| **01**    | Grayscale & Thresholding | Global Memory Coalescing              | _Pending_            |
| **02**    | Gaussian Blur            | Shared Memory Tiling & Halo Handling  | _Pending_            |
| **03**    | Histogram Calculation    | Atomic Operations & Local Reductions  | _Pending_            |
| **04**    | Harris Corner Detector   | CUDA Streams & Asynchronous Execution | _Pending_            |
| **05**    | BRIEF/ORB Matcher        | Warp Intrinsics (`__popc`, `__shfl`)  | _Pending_            |
| **06**    | Visual Odometry Pipeline | End-to-End Frame Tracking             | _Pending_            |

## 🛠️ Architecture

- **src/**: Contains isolated modules for each algorithmic stage.
- **common/**: Shared utilities for precise benchmarking.
- **data/**: Scripts and placeholders for the [KITTI Odometry Dataset](https://www.cvlibs.net/datasets/kitti/eval_odometry.php "null").

## ⚙️ Installation & Usage

### Option A: Local Development (GUI & Visualization)

_Recommended for debugging and visual verification of kernels._

1. **Install Dependencies** (Ubuntu 22.04):

   ```
   chmod +x scripts/install_deps.sh
   ./scripts/install_deps.sh
   ```

2. **Build:**

   ```
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

3. Run Visualization:

   This will open an OpenCV window showing the input vs. CUDA output.

   ```
   ./bin/01_pixel_ops_gui  # Example for Phase 1
   ```

### Option B: Docker (Terminal Benchmarking)

_Recommended for clean performance testing and portfolio demonstration._

1. **Build Image:**

   ```
   docker build -t cuda-vision-lab .
   ```

2. Run Benchmarks:

   This runs the performance suite without any GUI dependencies.

   ```
   docker run --gpus all cuda-vision-lab
   ```

## 📊 Performance Analysis Methodology

Each module includes a `benchmark` target that runs the algorithm on a standard image sequence (KITTI) 100 times to average the execution metrics.

```
// Example Metric Output
[Gaussian Blur 5x5]
CPU Time (OpenCV): 12.5 ms
GPU Time (Global):  3.2 ms
GPU Time (Shared):  0.8 ms
Speedup: 15.6x
```

## 👤 Author

**Alibek Erkabayev (bigalex95)** _Computer Vision & Machine Learning Engineer_ Specializing in High-Performance Computing and Embedded Vision.
