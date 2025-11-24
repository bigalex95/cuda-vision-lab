#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include <functional>
#include <cuda_runtime.h>
#include <iomanip>
#include <string>

// Terminal Colors
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

struct BenchmarkResult {
    float cpu_ms;
    float gpu_ms;
    float speedup;
};

// Measures CPU Execution Time (Wall Clock)
template <typename Func, typename... Args>
float benchmarkCPU(Func func, int iterations, Args&&... args) {
    // Warmup
    func(std::forward<Args>(args)...);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func(std::forward<Args>(args)...);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<float, std::milli> duration = end - start;
    return duration.count() / iterations;
}

// Measures GPU Kernel Execution Time (using CUDA Events)
template <typename Func>
float benchmarkGPU(Func gpu_wrapper, int iterations) {
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Warmup
    gpu_wrapper();
    cudaDeviceSynchronize();

    cudaEventRecord(start);
    for (int i = 0; i < iterations; ++i) {
        gpu_wrapper();
    }
    cudaEventRecord(stop);
    
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    
    return milliseconds / iterations;
}

void printStats(const std::string& name, BenchmarkResult res) {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "--------------------------------------------------\n";
    std::cout << CYAN << "Algorithm: " << name << RESET << "\n";
    std::cout << "CPU Time: " << res.cpu_ms << " ms\n";
    std::cout << "GPU Time: " << GREEN << res.gpu_ms << " ms" << RESET << "\n";
    
    if (res.speedup > 1.0f)
        std::cout << "Speedup:  " << YELLOW << res.speedup << "x" << RESET << "\n";
    else
        std::cout << "Speedup:  " << RED << res.speedup << "x (Slower)" << RESET << "\n";
        
    std::cout << "--------------------------------------------------\n";
}
