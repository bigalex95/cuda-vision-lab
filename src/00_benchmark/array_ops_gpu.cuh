#pragma once

// GPU implementations of basic array operations for benchmarking

void arrayAddGPU(const float *d_a, const float *d_b, float *d_c, int n);
void arrayMultiplyGPU(const float *d_a, const float *d_b, float *d_c, int n);
void arrayScaleGPU(const float *d_a, float scalar, float *d_out, int n);
float arrayDotProductGPU(const float *d_a, const float *d_b, int n);
void arrayNormalizeGPU(const float *d_in, float *d_out, int n);
