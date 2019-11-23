#pragma once

#include <benchmark/benchmark.h>

#include <core_types.h>
#include <templates/utility.h>

#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

void mat4f_transpose(float a[4][4])
{
	__m128 a0, a1, a2, a3;
	__m128 v0, v1, v2, v3;

    a0 = _mm_load_ps(a[0]);
	a1 = _mm_load_ps(a[1]);
	a2 = _mm_load_ps(a[2]);
	a3 = _mm_load_ps(a[3]);

    v0 = _mm_unpacklo_ps(a0, a2);
    v1 = _mm_unpacklo_ps(a1, a3);
    v2 = _mm_unpackhi_ps(a0, a2);
    v3 = _mm_unpackhi_ps(a1, a3);

    a0 = _mm_unpacklo_ps(v0, v1);
    a1 = _mm_unpackhi_ps(v0, v1);
    a2 = _mm_unpacklo_ps(v2, v3);
    a3 = _mm_unpackhi_ps(v2, v3);

	_mm_store_ps(a[0], a0);
	_mm_store_ps(a[1], a1);
	_mm_store_ps(a[2], a2);
	_mm_store_ps(a[3], a3);
}

void mat4f_mul_mat_s(float * __restrict__ a, const float * __restrict__ b)
{
	a = (float*)__builtin_assume_aligned(a, 32);
	b = (const float*)__builtin_assume_aligned(b, 32);

	for (int i = 0; i < 4; ++i)
	{
		alignas(32) float c[4] = {};

		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				c[j] += a[4 * i + k] * b[4 * k + j];
			}
		}

		memcpy(a + 4 * i, c, sizeof(c));
	}
}

void mat4f_mul_mat(float a[4][4], const float b[4][4])
{
    __m128 i, j, k, l;
    __m128 b0, b1, b2, b3;

    b0 = _mm_load_ps(b[0]);
    b1 = _mm_load_ps(b[1]);
    b2 = _mm_load_ps(b[2]);
    b3 = _mm_load_ps(b[3]);

    i = _mm_load_ps(a[0]);
    l = _mm_mul_ps(_mm_permute_ps(i, 0xff), b3);
    k = _mm_mul_ps(_mm_permute_ps(i, 0xaa), b2);
    j = _mm_mul_ps(_mm_permute_ps(i, 0x66), b1);
    i = _mm_mul_ps(_mm_permute_ps(i, 0x00), b0);

    k = _mm_add_ps(k, l);
    i = _mm_add_ps(i, j);
    _mm_store_ps(a[0], _mm_add_ps(i, k));

    i = _mm_load_ps(a[1]);
    l = _mm_mul_ps(_mm_permute_ps(i, 0xff), b3);
    k = _mm_mul_ps(_mm_permute_ps(i, 0xaa), b2);
    j = _mm_mul_ps(_mm_permute_ps(i, 0x66), b1);
    i = _mm_mul_ps(_mm_permute_ps(i, 0x00), b0);

    k = _mm_add_ps(k, l);
    i = _mm_add_ps(i, j);
    _mm_store_ps(a[1], _mm_add_ps(i, k));

    i = _mm_load_ps(a[2]);
    l = _mm_mul_ps(_mm_permute_ps(i, 0xff), b3);
    k = _mm_mul_ps(_mm_permute_ps(i, 0xaa), b2);
    j = _mm_mul_ps(_mm_permute_ps(i, 0x66), b1);
    i = _mm_mul_ps(_mm_permute_ps(i, 0x00), b0);

    k = _mm_add_ps(k, l);
    i = _mm_add_ps(i, j);
    _mm_store_ps(a[2], _mm_add_ps(i, k));

    i = _mm_load_ps(a[3]);
    l = _mm_mul_ps(_mm_permute_ps(i, 0xff), b3);
    k = _mm_mul_ps(_mm_permute_ps(i, 0xaa), b2);
    j = _mm_mul_ps(_mm_permute_ps(i, 0x66), b1);
    i = _mm_mul_ps(_mm_permute_ps(i, 0x00), b0);

    k = _mm_add_ps(k, l);
    i = _mm_add_ps(i, j);
    _mm_store_ps(a[3], _mm_add_ps(i, k));
}

void mat4f_inv_transform(float a[4][4])
{
	__m128 a0, a1, a2, a3;
	__m128 v0, v1, v2, v3, v4, v5, v6, v7;
	
    a0 = _mm_load_ps(a[0]);
	a1 = _mm_load_ps(a[1]);
	a2 = _mm_load_ps(a[2]);
	a3 = _mm_load_ps(a[3]);

    v0 = _mm_unpacklo_ps(a0, a2);
    v1 = _mm_unpacklo_ps(a1, a3);
    v2 = _mm_unpackhi_ps(a0, a2);
    v3 = _mm_unpackhi_ps(a1, a3);

    a0 = _mm_unpacklo_ps(v0, v1);
    a1 = _mm_unpackhi_ps(v0, v1);
    a2 = _mm_unpacklo_ps(v2, v3);
    a3 = _mm_unpackhi_ps(v2, v3);

	// Invert 3x3

	v0 = _mm_permute_ps(a1, 0xc9);
	v1 = _mm_permute_ps(a2, 0xd2);
	v0 = _mm_mul_ps(v0, v1);

	v2 = _mm_permute_ps(a1, 0xd2);
	v3 = _mm_permute_ps(a2, 0xc9);
	v1 = _mm_mul_ps(v2, v3);

	v4 = _mm_permute_ps(a0, 0xc9);
	v5 = _mm_permute_ps(a2, 0xd2);
	v2 = _mm_mul_ps(v4, v5);

	v0 = _mm_sub_ps(v0, v1);

	v6 = _mm_permute_ps(a0, 0xd2);
	v7 = _mm_permute_ps(a2, 0xc9);
	v3 = _mm_mul_ps(v6, v7);

	v4 = _mm_permute_ps(a0, 0xc9);
	v5 = _mm_permute_ps(a1, 0xd2);
	v4 = _mm_mul_ps(v4, v5);

	v1 = _mm_sub_ps(v3, v2); // -(v2 - v3)

	v6 = _mm_permute_ps(a0, 0xd2);
	v7 = _mm_permute_ps(a1, 0xc9);
	v5 = _mm_mul_ps(v6, v7);

	v7 = _mm_mul_ps(a0, v0);
	v7 = _mm_hadd_ps(v7, v7);
	v7 = _mm_hadd_ps(v7, v7);
	v7 = _mm_rcp_ps(v7);

	v2 = _mm_sub_ps(v4, v5);

	v0 = _mm_mul_ps(v0, v7);
	v1 = _mm_mul_ps(v1, v7);
	v2 = _mm_mul_ps(v2, v7);

	v4 = _mm_mul_ps(a3, v0);
	v5 = _mm_mul_ps(a3, v1);
	v6 = _mm_mul_ps(a3, v2);

	v4 = _mm_hadd_ps(v4, v5);
	v5 = _mm_hadd_ps(v6, _mm_xor_ps(v7, v7));

	v4 = _mm_hadd_ps(v4, v5);

    v4 = _mm_xor_ps(v4, _mm_set1_ps(-0.f));
	v0 = _mm_blend_ps(v0, _mm_permute_ps(v4, 0x3f), 0x8);
	v1 = _mm_blend_ps(v1, _mm_permute_ps(v4, 0x7f), 0x8);
	v2 = _mm_blend_ps(v2, _mm_permute_ps(v4, 0xbf), 0x8);

	_mm_store_ps(a[0], v0);
	_mm_store_ps(a[1], v1);
	_mm_store_ps(a[2], v2);
	// a[3] as is
}

void mat4f_transpose_scalar(float * __restrict__ a)
{
    a = (float*)__builtin_assume_aligned(a, 32);

	swap(a[1], a[4]);
	swap(a[2], a[8]);
	swap(a[3], a[12]);
	swap(a[6], a[9]);
	swap(a[7], a[13]);
	swap(a[11], a[14]);
}

void mat4f_inv_transform_unscaled(float * __restrict__ a)
{
    a = (float*)__builtin_assume_aligned(a, 32);

	swap(a[1], a[4]);
	swap(a[2], a[8]);
	swap(a[6], a[9]);

	a[3] = -a[3];
	a[7] = -a[7];
	a[11] = -a[11];
}

void mat4f_inv(float a[4][4])
{
	__m128 a0, a1, a2, a3;
	__m128 v0, v1, v2, v3, v4, v5, v6, v7;
	__m128 d;

	// Transpose matrix
	
    a0 = _mm_load_ps(a[0]);
	a1 = _mm_load_ps(a[1]);
	a2 = _mm_load_ps(a[2]);
	a3 = _mm_load_ps(a[3]);

    v0 = _mm_unpacklo_ps(a0, a2);
    v1 = _mm_unpacklo_ps(a1, a3);
    v2 = _mm_unpackhi_ps(a0, a2);
    v3 = _mm_unpackhi_ps(a1, a3);

    a0 = _mm_unpacklo_ps(v0, v1);
    a1 = _mm_unpackhi_ps(v0, v1);
    a2 = _mm_unpacklo_ps(v2, v3);
    a3 = _mm_unpackhi_ps(v2, v3);

	// Upper half

	v0 = _mm_permute_ps(a2, 0x9e);
	v1 = _mm_permute_ps(a3, 0x7b);
	v0 = _mm_mul_ps(v0, v1);
	
	v2 = _mm_permute_ps(a2, 0x7b);
	v3 = _mm_permute_ps(a3, 0x9e);
	v1 = _mm_mul_ps(v2, v3);

	v0 = _mm_sub_ps(v0, v1);

	v4 = _mm_permute_ps(a2, 0x33);
	v5 = _mm_permute_ps(a3, 0x8d);
	v2 = _mm_mul_ps(v4, v5);

	v6 = _mm_permute_ps(a2, 0x8d);
	v7 = _mm_permute_ps(a3, 0x33);
	v3 = _mm_mul_ps(v6, v7);
	
	v1 = _mm_sub_ps(v2, v3);

	v4 = _mm_permute_ps(a2, 0x49);
	v5 = _mm_permute_ps(a3, 0x12);
	v2 = _mm_mul_ps(v4, v5);

	v6 = _mm_permute_ps(a2, 0x12);
	v7 = _mm_permute_ps(a3, 0x49);
	v3 = _mm_mul_ps(v6, v7);

	v2 = _mm_sub_ps(v2, v3);

	v4 = _mm_permute_ps(a1, 0x01);
	v5 = _mm_permute_ps(a1, 0x5a);
	v4 = _mm_mul_ps(v4, v0);
	v5 = _mm_mul_ps(v5, v1);

	v6 = _mm_permute_ps(a1, 0xbf);
	v6 = _mm_mul_ps(v6, v2);

	v3 = _mm_add_ps(v4, v5);
	
	v4 = _mm_permute_ps(a0, 0x01);
	v5 = _mm_permute_ps(a0, 0x5a);
	v4 = _mm_mul_ps(v4, v0);
	v5 = _mm_mul_ps(v5, v1);

	v3 = _mm_add_ps(v3, v6);

	v6 = _mm_permute_ps(a0, 0xbf);
	v6 = _mm_mul_ps(v6, v2);

	d = _mm_mul_ps(a0, v3);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
	d = _mm_rcp_ps(d);

	v7 = _mm_add_ps(v4, v5);
	v7 = _mm_add_ps(v7, v6);
	v7 = _mm_xor_ps(v7, _mm_set1_ps(-0.f));

	_mm_store_ps(a[0], _mm_mul_ps(v3, d));
	_mm_store_ps(a[1], _mm_mul_ps(v7, d));
	
	// Lower half

	v0 = _mm_permute_ps(a0, 0x9e);
	v1 = _mm_permute_ps(a1, 0x7b);
	v0 = _mm_mul_ps(v0, v1);
	
	v2 = _mm_permute_ps(a0, 0x7b);
	v3 = _mm_permute_ps(a1, 0x9e);
	v1 = _mm_mul_ps(v2, v3);

	v4 = _mm_permute_ps(a0, 0x33);
	v5 = _mm_permute_ps(a1, 0x8d);
	v2 = _mm_mul_ps(v4, v5);

	v0 = _mm_sub_ps(v0, v1);

	v6 = _mm_permute_ps(a0, 0x8d);
	v7 = _mm_permute_ps(a1, 0x33);
	v3 = _mm_mul_ps(v6, v7);
	
	v1 = _mm_sub_ps(v2, v3);

	v4 = _mm_permute_ps(a0, 0x49);
	v5 = _mm_permute_ps(a1, 0x12);
	v2 = _mm_mul_ps(v4, v5);

	v6 = _mm_permute_ps(a0, 0x12);
	v7 = _mm_permute_ps(a1, 0x49);
	v3 = _mm_mul_ps(v6, v7);

	v4 = _mm_permute_ps(a3, 0x01);
	v5 = _mm_permute_ps(a3, 0x5a);
	v4 = _mm_mul_ps(v4, v0);
	v5 = _mm_mul_ps(v5, v1);

	v2 = _mm_sub_ps(v2, v3);

	v6 = _mm_permute_ps(a3, 0xbf);
	v6 = _mm_mul_ps(v6, v2);

	v3 = _mm_add_ps(v4, v5);
	
	v4 = _mm_permute_ps(a2, 0x01);
	v5 = _mm_permute_ps(a2, 0x5a);
	v4 = _mm_mul_ps(v4, v0);
	v5 = _mm_mul_ps(v5, v1);

	v3 = _mm_add_ps(v3, v6);

	v6 = _mm_permute_ps(a2, 0xbf);
	v6 = _mm_mul_ps(v6, v2);

	v7 = _mm_add_ps(v4, v5);
	v7 = _mm_add_ps(v7, v6);
	v7 = _mm_xor_ps(v7, _mm_set1_ps(-0.f));

	_mm_store_ps(a[2], _mm_mul_ps(v3, d));
	_mm_store_ps(a[3], _mm_mul_ps(v7, d));
}

bool gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

static void benchMat4Transpose(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (auto _ : state)
	{
		mat4f_transpose(a);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4Transpose);

static void benchMat4TransposeS(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (auto _ : state)
	{
		mat4f_transpose_scalar(a[0]);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4TransposeS);

static void benchMat4MulMat(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};
	alignas(32) float b[4][4] = {
		{2.f, 0.f, 0.f, 2.f},
		{0.f, 2.f, 0.f, 2.f},
		{0.f, 0.f, 2.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (auto _ : state)
	{
		mat4f_mul_mat(a, b);
	}
	
	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4MulMat);

static void benchMat4MulMatS(benchmark::State & state)
{
	alignas(32) float a[16] = {
		1.f, 0.f, 0.f, 2.f,
		0.f, 1.f, 0.f, 2.f,
		0.f, 0.f, 1.f, 2.f,
		0.f, 0.f, 0.f, 1.f
	};
	alignas(32) float b[16] = {
		2.f, 0.f, 0.f, 2.f,
		0.f, 2.f, 0.f, 2.f,
		0.f, 0.f, 2.f, 2.f,
		0.f, 0.f, 0.f, 1.f
	};

	for (auto _ : state)
	{
		mat4f_mul_mat_s(a, b);
	}
	
	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4MulMatS);

static void benchMat4Inv(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (int i = 0; i < 16; ++i)
		a[i / 4][i % 4] = rand() / (float)RAND_MAX;

	for (auto _ : state)
	{
		mat4f_inv(a);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4Inv);

static void benchMat4InvTransform(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	/* for (int i = 0; i < 16; ++i)
		a[i / 4][i % 4] = rand() / (float)RAND_MAX; */

	for (auto _ : state)
	{
		mat4f_inv_transform_unscaled(a[0]);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4InvTransform);

static void benchMat4InvGlu(benchmark::State & state)
{
	alignas(32) float a[16] = {
		1.f, 0.f, 0.f, 2.f,
		0.f, 1.f, 0.f, 2.f,
		0.f, 0.f, 1.f, 2.f,
		0.f, 0.f, 0.f, 1.f
	};
	alignas(32) float b[16];

	for (int i = 0; i < 16; ++i)
		a[i] = rand() / (float)RAND_MAX;

	for (auto _ : state)
	{
		gluInvertMatrix(a, b);
	}

	doNotOptimizeAway(b);
}
BENCHMARK(benchMat4InvGlu);

static void benchMat4(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};
	alignas(32) float b[4][4] = {
		{2.f, 0.f, 0.f, 2.f},
		{0.f, 2.f, 0.f, 2.f},
		{0.f, 0.f, 2.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (auto _ : state)
	{
		mat4f_transpose(a);
		mat4f_mul_mat(a, b);
		mat4f_inv_transform(a);
		mat4f_mul_mat(a, b);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4);

static void benchMat4S(benchmark::State & state)
{
	alignas(32) float a[4][4] = {
		{1.f, 0.f, 0.f, 2.f},
		{0.f, 1.f, 0.f, 2.f},
		{0.f, 0.f, 1.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};
	alignas(32) float b[4][4] = {
		{2.f, 0.f, 0.f, 2.f},
		{0.f, 2.f, 0.f, 2.f},
		{0.f, 0.f, 2.f, 2.f},
		{0.f, 0.f, 0.f, 1.f}
	};

	for (auto _ : state)
	{
		mat4f_transpose_scalar(a[0]);
		mat4f_mul_mat_s(a[0], b[0]);
		gluInvertMatrix(a[0], b[0]);
		mat4f_mul_mat_s(a[0], b[0]);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchMat4S);

void vec3f_mul_vec_loop(float * __restrict__ a, float * __restrict__ b)
{
    a = (float*)__builtin_assume_aligned(a, 16);
    b = (float*)__builtin_assume_aligned(b, 16);

    for (int i = 0; i < 3; ++i)
        a[i] *= b[i];
}

void vec4f_mul_vec_loop(float * __restrict__ a, float * __restrict__ b)
{
    a = (float*)__builtin_assume_aligned(a, 16);
    b = (float*)__builtin_assume_aligned(b, 16);

    for (int i = 0; i < 4; ++i)
        a[i] *= b[i];
}

static void benchVec4MulS(benchmark::State & state)
{
	alignas(16) float a[4] = {1.f, 2.f, 0.5f, 3.f};
	alignas(16) float b[4] = {15.f, 1.f, 0.2f, 2.5f};

	for (auto _ : state)
	{
		for (uint32 i = 0; i < 10; ++i)
			vec4f_mul_vec_loop(a, b);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchVec4MulS);

static void benchVec4Mul(benchmark::State & state)
{
	alignas(16) float a[4] = {1.f, 2.f, 0.5f, 3.f};
	alignas(16) float b[4] = {15.f, 1.f, 0.2f, 2.5f};

	for (auto _ : state)
	{
		__m128 _a = _mm_load_ps(a), _b = _mm_load_ps(b);

		for (uint32 i = 0; i < 10; ++i)
			_a = _mm_mul_ps(_a, _b);

		_mm_store_ps(a, _a);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchVec4Mul);

static void benchVec3MulS(benchmark::State & state)
{
	alignas(16) float a[3] = {1.f, 2.f, 0.5f};
	alignas(16) float b[3] = {15.f, 1.f, 0.2f};

	for (auto _ : state)
	{
		for (uint32 i = 0; i < 10; ++i)
			vec3f_mul_vec_loop(a, b);
	}

	doNotOptimizeAway(a);
}
BENCHMARK(benchVec3MulS);