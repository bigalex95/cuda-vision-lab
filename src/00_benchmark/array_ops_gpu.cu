#include "array_ops_gpu.cuh"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cmath>
#include <vector>

// Kernel: Element-wise addition
__global__ void arrayAddKernel(const float *a, const float *b, float *c, int n)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n)
    {
        c[idx] = a[idx] + b[idx];
    }
}

// Kernel: Element-wise multiplication
__global__ void arrayMultiplyKernel(const float *a, const float *b, float *c, int n)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n)
    {
        c[idx] = a[idx] * b[idx];
    }
}

// Kernel: Scalar multiplication
__global__ void arrayScaleKernel(const float *a, float scalar, float *out, int n)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n)
    {
        out[idx] = a[idx] * scalar;
    }
}

// Kernel: Reduction for dot product
__global__ void dotProductKernel(const float *a, const float *b, float *partial_sums, int n)
{
    __shared__ float shared_data[256];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Each thread computes one product
    shared_data[tid] = (idx < n) ? a[idx] * b[idx] : 0.0f;
    __syncthreads();

    // Reduction in shared memory
    for (int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shared_data[tid] += shared_data[tid + s];
        }
        __syncthreads();
    }

    // Write result for this block to global memory
    if (tid == 0)
    {
        partial_sums[blockIdx.x] = shared_data[0];
    }
}

// Kernel: Compute squared magnitude for normalization
__global__ void computeMagnitudeKernel(const float *in, float *partial_mags, int n)
{
    __shared__ float shared_data[256];

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    float val = (idx < n) ? in[idx] : 0.0f;
    shared_data[tid] = val * val;
    __syncthreads();

    // Reduction
    for (int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shared_data[tid] += shared_data[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0)
    {
        partial_mags[blockIdx.x] = shared_data[0];
    }
}

// Kernel: Normalize array by magnitude
__global__ void normalizeKernel(const float *in, float *out, float magnitude, int n)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n)
    {
        out[idx] = in[idx] / magnitude;
    }
}

// Host functions

void arrayAddGPU(const float *d_a, const float *d_b, float *d_c, int n)
{
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    arrayAddKernel<<<blocks, threads>>>(d_a, d_b, d_c, n);
}

void arrayMultiplyGPU(const float *d_a, const float *d_b, float *d_c, int n)
{
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    arrayMultiplyKernel<<<blocks, threads>>>(d_a, d_b, d_c, n);
}

void arrayScaleGPU(const float *d_a, float scalar, float *d_out, int n)
{
    int threads = 256;
    int blocks = (n + threads - 1) / threads;
    arrayScaleKernel<<<blocks, threads>>>(d_a, scalar, d_out, n);
}

float arrayDotProductGPU(const float *d_a, const float *d_b, int n)
{
    int threads = 256;
    int blocks = (n + threads - 1) / threads;

    // Allocate memory for partial sums
    float *d_partial_sums;
    cudaMalloc(&d_partial_sums, blocks * sizeof(float));

    // First reduction
    dotProductKernel<<<blocks, threads>>>(d_a, d_b, d_partial_sums, n);

    // Copy partial sums to host and compute final sum
    std::vector<float> h_partial_sums(blocks);
    cudaMemcpy(h_partial_sums.data(), d_partial_sums, blocks * sizeof(float), cudaMemcpyDeviceToHost);

    float result = 0.0f;
    for (int i = 0; i < blocks; ++i)
    {
        result += h_partial_sums[i];
    }

    cudaFree(d_partial_sums);
    return result;
}

void arrayNormalizeGPU(const float *d_in, float *d_out, int n)
{
    int threads = 256;
    int blocks = (n + threads - 1) / threads;

    // Compute magnitude
    float *d_partial_mags;
    cudaMalloc(&d_partial_mags, blocks * sizeof(float));

    computeMagnitudeKernel<<<blocks, threads>>>(d_in, d_partial_mags, n);

    std::vector<float> h_partial_mags(blocks);
    cudaMemcpy(h_partial_mags.data(), d_partial_mags, blocks * sizeof(float), cudaMemcpyDeviceToHost);

    float mag_squared = 0.0f;
    for (int i = 0; i < blocks; ++i)
    {
        mag_squared += h_partial_mags[i];
    }
    float magnitude = std::sqrt(mag_squared);

    cudaFree(d_partial_mags);

    // Normalize
    if (magnitude > 1e-6f)
    {
        normalizeKernel<<<blocks, threads>>>(d_in, d_out, magnitude, n);
    }
}
