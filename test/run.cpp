/**
 * This file contains a sandbox for
 * testing the SGL library
 */
#include "core_types.h"

#include "math/math_types.h"
#include "math/vec2.h"
#include "math/vec3.h"

int main()
{
	vec3 location = {};
	vec3 velocity = {};
	vec3 acceleration = {1.f, 0.f, 1.f};

	uint64 currTick = clock(), prevTick;

	for (;;)
	{
		// Update time
		prevTick = currTick;
		currTick = clock();
		float32 dt = (currTick - prevTick) / (float32)CLOCKS_PER_SEC;

		printf("dt: %f\n", dt);

		velocity += acceleration * dt;
		location += velocity * dt;

		printf("v3(%f, %f, %f)\n", location.x, location.y, location.z);
	}

	return 0;
}