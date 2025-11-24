#include "array_ops_cpu.h"
#include "array_ops_gpu.cuh"
#include "../common/benchmark_utils.h"
#include <iostream>
#include <vector>
#include <random>
#include <cuda_runtime.h>

// Initialize random array
void initRandomArray(float *arr, int n)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    for (int i = 0; i < n; ++i)
    {
        arr[i] = dis(gen);
    }
}

// Verify results match (with tolerance)
bool verifyResults(const float *cpu_result, const float *gpu_result, int n, float tolerance = 1e-3f)
{
    for (int i = 0; i < n; ++i)
    {
        if (std::abs(cpu_result[i] - gpu_result[i]) > tolerance)
        {
            std::cout << RED << "❌ Mismatch at index " << i
                      << ": CPU=" << cpu_result[i]
                      << " GPU=" << gpu_result[i] << RESET << "\n";
            return false;
        }
    }
    return true;
}

int main()
{
    // Array sizes to test
    std::vector<int> sizes = {1000000, 10000000, 50000000};
    const int iterations = 100;

    std::cout << "\n";
    std::cout << CYAN << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║   CUDA Vision Lab - Benchmark Suite           ║\n";
    std::cout << "║   CPU vs GPU Array Operations                  ║\n";
    std::cout << "╚════════════════════════════════════════════════╝" << RESET << "\n\n";

    // Query GPU properties
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    std::cout << "GPU: " << prop.name << "\n";
    std::cout << "Compute Capability: " << prop.major << "." << prop.minor << "\n";
    std::cout << "Global Memory: " << prop.totalGlobalMem / (1024 * 1024 * 1024) << " GB\n\n";

    for (int n : sizes)
    {
        std::cout << "\n"
                  << YELLOW << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "Testing with array size: " << n << " elements\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << "\n\n";

        // Allocate host memory
        std::vector<float> h_a(n), h_b(n), h_c_cpu(n), h_c_gpu(n);
        initRandomArray(h_a.data(), n);
        initRandomArray(h_b.data(), n);

        // Allocate device memory
        float *d_a, *d_b, *d_c;
        cudaMalloc(&d_a, n * sizeof(float));
        cudaMalloc(&d_b, n * sizeof(float));
        cudaMalloc(&d_c, n * sizeof(float));

        // Copy data to device
        cudaMemcpy(d_a, h_a.data(), n * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_b, h_b.data(), n * sizeof(float), cudaMemcpyHostToDevice);

        // ========== BENCHMARK: Array Addition ==========
        {
            float cpu_time = benchmarkCPU([&]()
                                          { arrayAddCPU(h_a.data(), h_b.data(), h_c_cpu.data(), n); }, iterations);

            float gpu_time = benchmarkGPU([&]()
                                          { arrayAddGPU(d_a, d_b, d_c, n); }, iterations);

            // Verify correctness
            cudaMemcpy(h_c_gpu.data(), d_c, n * sizeof(float), cudaMemcpyDeviceToHost);
            bool correct = verifyResults(h_c_cpu.data(), h_c_gpu.data(), n);

            BenchmarkResult result = {cpu_time, gpu_time, cpu_time / gpu_time};
            printStats("Array Addition", result);
            if (correct)
            {
                std::cout << GREEN << "✓ Results verified correct\n"
                          << RESET;
            }
            std::cout << "\n";
        }

        // ========== BENCHMARK: Array Multiplication ==========
        {
            float cpu_time = benchmarkCPU([&]()
                                          { arrayMultiplyCPU(h_a.data(), h_b.data(), h_c_cpu.data(), n); }, iterations);

            float gpu_time = benchmarkGPU([&]()
                                          { arrayMultiplyGPU(d_a, d_b, d_c, n); }, iterations);

            cudaMemcpy(h_c_gpu.data(), d_c, n * sizeof(float), cudaMemcpyDeviceToHost);
            bool correct = verifyResults(h_c_cpu.data(), h_c_gpu.data(), n);

            BenchmarkResult result = {cpu_time, gpu_time, cpu_time / gpu_time};
            printStats("Array Multiplication", result);
            if (correct)
            {
                std::cout << GREEN << "✓ Results verified correct\n"
                          << RESET;
            }
            std::cout << "\n";
        }

        // ========== BENCHMARK: Scalar Multiplication ==========
        {
            float scalar = 2.5f;
            float cpu_time = benchmarkCPU([&]()
                                          { arrayScaleCPU(h_a.data(), scalar, h_c_cpu.data(), n); }, iterations);

            float gpu_time = benchmarkGPU([&]()
                                          { arrayScaleGPU(d_a, scalar, d_c, n); }, iterations);

            cudaMemcpy(h_c_gpu.data(), d_c, n * sizeof(float), cudaMemcpyDeviceToHost);
            bool correct = verifyResults(h_c_cpu.data(), h_c_gpu.data(), n);

            BenchmarkResult result = {cpu_time, gpu_time, cpu_time / gpu_time};
            printStats("Scalar Multiplication", result);
            if (correct)
            {
                std::cout << GREEN << "✓ Results verified correct\n"
                          << RESET;
            }
            std::cout << "\n";
        }

        // ========== BENCHMARK: Dot Product ==========
        {
            float cpu_result, gpu_result;

            float cpu_time = benchmarkCPU([&]()
                                          { cpu_result = arrayDotProductCPU(h_a.data(), h_b.data(), n); }, iterations);

            float gpu_time = benchmarkGPU([&]()
                                          { gpu_result = arrayDotProductGPU(d_a, d_b, n); }, iterations);

            bool correct = std::abs(cpu_result - gpu_result) < 1e-1f; // Larger tolerance for reductions

            BenchmarkResult result = {cpu_time, gpu_time, cpu_time / gpu_time};
            printStats("Dot Product", result);
            if (correct)
            {
                std::cout << GREEN << "✓ Results verified correct (CPU: " << cpu_result
                          << ", GPU: " << gpu_result << ")\n"
                          << RESET;
            }
            else
            {
                std::cout << RED << "❌ Mismatch: CPU=" << cpu_result
                          << " GPU=" << gpu_result << RESET << "\n";
            }
            std::cout << "\n";
        }

        // ========== BENCHMARK: Array Normalization ==========
        {
            float cpu_time = benchmarkCPU([&]()
                                          { arrayNormalizeCPU(h_a.data(), h_c_cpu.data(), n); }, iterations);

            float gpu_time = benchmarkGPU([&]()
                                          { arrayNormalizeGPU(d_a, d_c, n); }, iterations);

            cudaMemcpy(h_c_gpu.data(), d_c, n * sizeof(float), cudaMemcpyDeviceToHost);
            bool correct = verifyResults(h_c_cpu.data(), h_c_gpu.data(), n, 1e-2f);

            BenchmarkResult result = {cpu_time, gpu_time, cpu_time / gpu_time};
            printStats("Array Normalization", result);
            if (correct)
            {
                std::cout << GREEN << "✓ Results verified correct\n"
                          << RESET;
            }
            std::cout << "\n";
        }

        // Cleanup
        cudaFree(d_a);
        cudaFree(d_b);
        cudaFree(d_c);
    }

    std::cout << "\n"
              << CYAN << "Benchmark suite complete! ✓" << RESET << "\n\n";

    return 0;
}
