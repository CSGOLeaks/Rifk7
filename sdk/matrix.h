#pragma once

struct viewmatrix
{
    float matrix[4][4];

    __forceinline float* operator[](const int i)
    {
        return matrix[i];
    }

    __forceinline const float* operator[](const int i) const
    {
        return matrix[i];
    }
};

struct matrix3x4
{
    float matrix[3][4];

    __forceinline float* operator[](const int i)
    {
        return matrix[i];
    }

    __forceinline const float* operator[](const int i) const
    {
        return matrix[i];
    }
};
