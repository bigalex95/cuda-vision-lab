# Common Utilities Guide

## Overview

The `common/` directory contains reusable header-only utilities for benchmarking and visualization.

## Available Headers

### `benchmark_utils.h`

Performance benchmarking for CPU and GPU operations.

**Usage:**

```cpp
#include "../common/benchmark_utils.h"

// Benchmark CPU
float cpu_time = benchmarkCPU([&]() {
    myCPUFunction();
}, 100);  // 100 iterations

// Benchmark GPU
float gpu_time = benchmarkGPU([&]() {
    myGPUKernel<<<grid, block>>>();
}, 100);

// Print results
BenchmarkResult result = {cpu_time, gpu_time, cpu_time/gpu_time};
printStats("Algorithm Name", result);
```

**Key Functions:**

- `benchmarkCPU(func, iterations)` - Measures CPU execution time
- `benchmarkGPU(func, iterations)` - Measures GPU execution time with CUDA events
- `printStats(name, result)` - Prints formatted benchmark results

---

### `visualization_utils.h`

Image visualization and comparison tools for debugging and analysis.

**Usage:**

```cpp
#include "../common/visualization_utils.h"
using namespace viz;

// Load and display
cv::Mat img = loadImage("input.jpg");
showImage("Title", img);

// Compare CPU vs GPU
cv::Mat comparison = createComparison(cpu_result, gpu_result, "CPU", "GPU");
cv::Mat with_perf = addPerformanceOverlay(comparison, cpu_time, gpu_time, "Algorithm");
showImage("Results", with_perf);
waitForKey();

// Analyze differences
cv::Mat diff = visualizeDifference(img1, img2, 10.0f);
double psnr, mse;
calculateImageQuality(img1, img2, psnr, mse);

// Save results
saveImage("output.png", with_perf);
```

**Key Functions:**

| Category        | Functions                                                               |
| --------------- | ----------------------------------------------------------------------- |
| **Display**     | `showImage()`, `showImages()`, `waitForKey()`, `closeAllWindows()`      |
| **Comparison**  | `createComparison()`, `createGrid()`                                    |
| **Analysis**    | `visualizeDifference()`, `calculateImageQuality()`, `printImageStats()` |
| **Features**    | `drawKeypoints()`, `drawMatches()`                                      |
| **Performance** | `addPerformanceOverlay()`                                               |
| **I/O**         | `loadImage()`, `saveImage()`                                            |

---

## Complete Example

```cpp
#include "../common/benchmark_utils.h"
#include "../common/visualization_utils.h"
#include <opencv2/opencv.hpp>

void processCPU(const cv::Mat& input, cv::Mat& output) {
    cv::GaussianBlur(input, output, cv::Size(5, 5), 0);
}

void processGPU(const cv::Mat& input, cv::Mat& output) {
    // Your CUDA implementation here
    processCPU(input, output);  // placeholder
}

int main() {
    // Load input
    cv::Mat input = viz::loadImage("data/image.jpg", cv::IMREAD_GRAYSCALE);
    if (input.empty()) return -1;

    // Process and benchmark
    cv::Mat cpu_result, gpu_result;
    float cpu_time = benchmarkCPU([&]() {
        processCPU(input, cpu_result);
    }, 100);

    float gpu_time = benchmarkGPU([&]() {
        processGPU(input, gpu_result);
    }, 100);

    // Print console stats
    BenchmarkResult result = {cpu_time, gpu_time, cpu_time/gpu_time};
    printStats("Gaussian Blur", result);

    // Visualize results
    std::vector<cv::Mat> stages = {input, cpu_result, gpu_result};
    std::vector<std::string> labels = {"Input", "CPU", "GPU"};
    cv::Mat grid = viz::createGrid(stages, labels, 3);
    cv::Mat final = viz::addPerformanceOverlay(grid, cpu_time, gpu_time, "Gaussian Blur 5x5");

    // Quality check
    double psnr, mse;
    viz::calculateImageQuality(cpu_result, gpu_result, psnr, mse);
    std::cout << "PSNR: " << psnr << " dB, MSE: " << mse << std::endl;

    // Display and save
    viz::showImage("Results", final);
    viz::waitForKey();
    viz::saveImage("output/result.png", final);

    return 0;
}
```

## Integration in CMakeLists.txt

```cmake
# Include common utilities
include_directories(${CMAKE_SOURCE_DIR}/common)

# Link CUDA for GPU benchmarking
find_package(CUDAToolkit REQUIRED)
target_link_libraries(your_target
    ${OpenCV_LIBS}
    CUDA::cudart
)
```

## Tips

1. **Header-only** - No compilation needed, just include
2. **Namespace** - All viz functions are in `viz::` namespace
3. **Error handling** - Functions check for empty images and invalid inputs
4. **Color coded** - Console output uses colors: Green (success), Red (errors), Yellow (info), Cyan (headers)
5. **CUDA optional** - `cudaToMat()` only available when compiling with CUDA

## Dependencies

- OpenCV 4.x (required)
- CUDA Runtime (required for GPU benchmarking)
- C++17 (required)
