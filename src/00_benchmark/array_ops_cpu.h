#pragma once

// CPU implementations of basic array operations for benchmarking

void arrayAddCPU(const float *a, const float *b, float *c, int n);
void arrayMultiplyCPU(const float *a, const float *b, float *c, int n);
void arrayScaleCPU(const float *a, float scalar, float *out, int n);
float arrayDotProductCPU(const float *a, const float *b, int n);
void arrayNormalizeCPU(const float *in, float *out, int n);
