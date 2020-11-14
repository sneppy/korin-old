#pragma once

#include "core_types.h"

//////////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////////

template<typename T>	struct Vec2;
template<typename T>	struct Vec3;
template<typename T>	struct Vec4;
						struct Quat;
template<typename T>	struct Vector;
template<typename T>	struct Mat3;
template<typename T>	struct Mat4;
template<typename T>	struct Matrix;

//////////////////////////////////////////////////
// Type aliases
//////////////////////////////////////////////////

using vec2 = Vec2<float32>;
using vec3 = Vec3<float32>;
using vec4 = Vec4<float32>;

using float2 = Vec2<float32>;
using float3 = Vec3<float32>;
using float4 = Vec4<float32>;

using ivec2 = Vec2<int32>;
using ivec3 = Vec3<int32>;
using ivec4 = Vec4<int32>;

using int2 = Vec2<int32>;
using int3 = Vec3<int32>;
using int4 = Vec4<int32>;

using uvec2 = Vec2<uint32>;
using uvec3 = Vec3<uint32>;
using uvec4 = Vec4<uint32>;

using uint2 = Vec2<uint32>;
using uint3 = Vec3<uint32>;
using uint4 = Vec4<uint32>;

///////////////////////////

using mat3 = Mat3<float32>;
using mat4 = Mat4<float32>;

using float3x3 = Mat3<float32>;
using float4x4 = Mat4<float32>;

using imat3 = Mat3<int32>;
using imat4 = Mat4<int32>;

using int3x3 = Mat4<int32>;
using int4x4 = Mat4<int32>;

using umat3 = Mat3<uint32>;
using umat4 = Mat4<uint32>;

using uint3x3 = Mat4<uint32>;
using uint4x4 = Mat4<uint32>;