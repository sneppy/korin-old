#pragma once

#include "core_types.h"

//////////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////////

template<typename T> struct Vec2;
template<typename T> struct Vec3;
template<typename T> struct Vec4;

//////////////////////////////////////////////////
// Type aliases
//////////////////////////////////////////////////

using vec2 = Vec2<float32>;
using vec3 = Vec3<float32>;
using vec4 = Vec4<float32>;

using ivec2 = Vec2<int32>;
using ivec3 = Vec3<int32>;
using ivec4 = Vec4<int32>;

using uvec2 = Vec2<uint32>;
using uvec3 = Vec3<uint32>;
using uvec4 = Vec4<uint32>;