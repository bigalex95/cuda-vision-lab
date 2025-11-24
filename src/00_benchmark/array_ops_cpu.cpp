#include "array_ops_cpu.h"
#include <cmath>

void arrayAddCPU(const float *a, const float *b, float *c, int n)
{
    for (int i = 0; i < n; ++i)
    {
        c[i] = a[i] + b[i];
    }
}

void arrayMultiplyCPU(const float *a, const float *b, float *c, int n)
{
    for (int i = 0; i < n; ++i)
    {
        c[i] = a[i] * b[i];
    }
}

void arrayScaleCPU(const float *a, float scalar, float *out, int n)
{
    for (int i = 0; i < n; ++i)
    {
        out[i] = a[i] * scalar;
    }
}

float arrayDotProductCPU(const float *a, const float *b, int n)
{
    float sum = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

void arrayNormalizeCPU(const float *in, float *out, int n)
{
    // Compute magnitude
    float mag = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        mag += in[i] * in[i];
    }
    mag = std::sqrt(mag);

    // Normalize
    if (mag > 1e-6f)
    {
        for (int i = 0; i < n; ++i)
        {
            out[i] = in[i] / mag;
        }
    }
}
