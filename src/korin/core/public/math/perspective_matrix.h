#pragma once

#include "core_types.h"
#include "hal/platform_math.h"
#include "./math_types.h"
#include "./mat4.h"

/**
 * @brief A 4x4 matrix that represents
 * a perspective transformation.
 */
struct PerspectiveMatrix : Mat4<float32>
{
	/**
	 * @brief Create a perspective matrix given
	 * projection box extents.
	 */
	constexpr FORCE_INLINE PerspectiveMatrix(float32 right, float32 left, float32 top, float32 bottom, float32 nearZ, float32 farZ)
		: Mat4{
			(2.f * nearZ) / (right - left), 0.f, (right + left) / (left - right), 0.f,
			0.f, (2.f * nearZ) / (top - bottom), (top + bottom) / (bottom - top), 0.f,
			0.f, 0.f, (nearZ + farZ) / (nearZ - farZ), (2.f * nearZ * farZ) / (nearZ - farZ),
			0.f, 0.f, 1.f, 0.f
		}
	{
		//
	}

	/**
	 * @brief Initialize the perspective matrix
	 * with a 16:9 aspect ratio and a 90 degrees
	 * FOV. 
	 */
	constexpr FORCE_INLINE PerspectiveMatrix()
		: PerspectiveMatrix{8.f, -8.f, 4.5f, -4.5f, 0.5f, 1000.f}
	{
		//
	}

private:
	using Mat4::Mat4;

	/**
	 * @brief Internal construction method. 
	 */
	constexpr PerspectiveMatrix(float32 ratio, float32 invfov, float32 nearZ, float32 farZ)
		: Mat4{
			invfov, 0.f, 0.f, 0.f,
			0.f, invfov * ratio, 0.f, 0.f,
			0.f, 0.f, (nearZ + farZ) / (nearZ - farZ), (2.f * nearZ * farZ) / (nearZ - farZ),
			0.f, 0.f, 1.f, 0.f
		}
	{
		//
	}

public:
	/**
	 * @brief Creates a new perspective matrix.
	 * 
	 * @param width screen width
	 * @param height screen height
	 * @param fov camera field of view
	 * @param nearZ distance of the near plane
	 * @param farZ distance of the far plane
	 */
	constexpr FORCE_INLINE explicit PerspectiveMatrix(float32 width, float32 height, float32 fov, float32 nearZ, float32 farZ)
		: PerspectiveMatrix{width / height, 1.f / PlatformMath::tan(fov / 2.f), nearZ, farZ}
	{
		//
	}
};
