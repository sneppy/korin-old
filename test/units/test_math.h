#pragma once

#include <gtest/gtest.h>

#include "core_types.h"
#include "hal/platform_math.h"
#include "math/math_types.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/quat.h"

TEST(math, general)
{
	// Align down
	ASSERT_EQ(PlatformMath::align2Down(16, 16), 16);
	ASSERT_EQ(PlatformMath::align2Down(15, 8), 8);
	ASSERT_EQ(PlatformMath::align2Down(0, 32), 0);
	ASSERT_EQ(PlatformMath::align2Down(16 - 1, 8), 8);

	// Align up
	ASSERT_EQ(PlatformMath::align2Up(16, 16), 32);
	ASSERT_EQ(PlatformMath::align2Up(15, 8), 16);
	ASSERT_EQ(PlatformMath::align2Up(0, 32), 32);
	ASSERT_EQ(PlatformMath::align2Up(16 - 1, 8), 16);

	SUCCEED();
}

TEST(math, vec2)
{
	Vec2<float32> p = {0.5f, 1.25f};
	
	ASSERT_FLOAT_EQ(p.getSquaredSize(), 0.5f * 0.5f + 1.25f * 1.25f);
	ASSERT_FLOAT_EQ((-p).getSquaredSize(), p.getSquaredSize());

	ASSERT_FLOAT_EQ(p.getNormal().getSquaredSize(), 1.f);

	p.normalize();

	ASSERT_FLOAT_EQ(p.getSquaredSize(), 1.f);

	p = Vec2<float32>{3.f, 4.f};

	ASSERT_FLOAT_EQ(p.getSquaredSize(), 25.f);
	ASSERT_FLOAT_EQ(p.getSize(), 5.f);

	p += Vec2<float32>{2.f, 5.f};

	ASSERT_FLOAT_EQ(p.x, 5.f);
	ASSERT_FLOAT_EQ(p.y, 9.f);

	p -= Vec2<float32>{4.f, 7.f};

	ASSERT_FLOAT_EQ(p.x, 1.f);
	ASSERT_FLOAT_EQ(p.y, 2.f);

	p *= Vec2<float32>{4.f, 2.f};

	ASSERT_FLOAT_EQ(p.x, 4.f);
	ASSERT_FLOAT_EQ(p.y, 4.f);

	p /= Vec2<float32>{8.f, 4.f};

	ASSERT_FLOAT_EQ(p.x, 0.5f);
	ASSERT_FLOAT_EQ(p.y, 1.f);

	p += 4.f;

	ASSERT_FLOAT_EQ(p.x, 4.5f);
	ASSERT_FLOAT_EQ(p.y, 5.f);

	p -= 2.f;

	ASSERT_FLOAT_EQ(p.x, 2.5f);
	ASSERT_FLOAT_EQ(p.y, 3.f);

	p *= 10.f;

	ASSERT_FLOAT_EQ(p.x, 25.f);
	ASSERT_FLOAT_EQ(p.y, 30.f);

	p /= 2;

	ASSERT_FLOAT_EQ(p.x, 12.5f);
	ASSERT_FLOAT_EQ(p.y, 15.f);

	float32 dot = p & Vec2<float32>{0.5f, 1.f};

	ASSERT_FLOAT_EQ(dot, 12.5f * 0.5f + 15.f * 1.f);

	Vec3<float32> cross = p ^ Vec2<float32>{0.5f, 1.f};

	ASSERT_FLOAT_EQ(cross.z, 12.5f * 1.f - 15.f * 0.5f);

	Vec2<float32> a{1.f, 2.f}, b{5.f, 1.f}, c = a;

	ASSERT_EQ(a.cmpeq(b), 0x0);
	ASSERT_EQ(a.cmpeq(c), 0x3);
	ASSERT_EQ(a.cmpeq(c), c.cmpeq(a));
	ASSERT_EQ(a.cmpne(b), 0x3);
	ASSERT_EQ(a.cmpne(c), 0x0);
	ASSERT_EQ(a.cmpne(c), c.cmpne(a));
	ASSERT_EQ(a.cmplt(b), 0x1);
	ASSERT_EQ(a.cmplt(c), 0x0);
	ASSERT_EQ(a.cmpgt(b), 0x2);
	ASSERT_EQ(a.cmpgt(c), 0x0);
	ASSERT_EQ(a.cmple(b), 0x1);
	ASSERT_EQ(a.cmple(c), 0x3);
	ASSERT_EQ(a.cmpge(b), 0x2);
	ASSERT_EQ(a.cmpge(c), 0x3);

	Vec2<int32> point = p;
	
	ASSERT_EQ(point.x, 12);
	ASSERT_EQ(point.y, 15);

	SUCCEED();
}

TEST(math, vec3)
{
	Vec3<float32> a, b, c, d;

	a = Vec3<float32>{0.4f, 0.3f, 0.f};

	ASSERT_FLOAT_EQ(a.getSquaredSize(), 0.4f * 0.4f + 0.3f * 0.3f);
	ASSERT_FLOAT_EQ(a.getSize(), 0.5f);
	ASSERT_FLOAT_EQ((-a).getSquaredSize(), a.getSquaredSize());

	b = a / a.getSize();

	ASSERT_FLOAT_EQ(b.x, 0.8f);
	ASSERT_FLOAT_EQ(b.y, 0.6f);
	ASSERT_FLOAT_EQ(b.z, 0.f);

	ASSERT_FLOAT_EQ(a.getNormal().getSquaredSize(), 1.f);

	a.normalize();

	ASSERT_FLOAT_EQ(a.getSquaredSize(), 1.f);

	a = Vec3<float32>{0.4f, 0.3f, 0.f};
	b += a;

	ASSERT_FLOAT_EQ(b.x, 1.2f);
	ASSERT_FLOAT_EQ(b.y, 0.9f);
	ASSERT_FLOAT_EQ(b.z, 0.f);

	c = Vec3<float32>{0.5, 2.f, 1.f};
	b *= c;

	ASSERT_FLOAT_EQ(b.x, 0.6f);
	ASSERT_FLOAT_EQ(b.y, 1.8f);
	ASSERT_FLOAT_EQ(b.z, 0.f);

	b.x = 0.6f;
	b.y = 1.8f;
	b.z = 0.f;
	b -= c;

	ASSERT_FLOAT_EQ(b.x, 0.1f);
	ASSERT_FLOAT_EQ(b.y, -0.2f);
	ASSERT_FLOAT_EQ(b.z, -1.f);

	b /= b;

	ASSERT_FLOAT_EQ(b.x, 1.f);
	ASSERT_FLOAT_EQ(b.y, 1.f);
	ASSERT_FLOAT_EQ(b.z, 1.f);

	a = Vec3<float32>{0.5f, 2.f, 3.f};
	b = Vec3<float32>{2.f, 5.f, 1.5f};

	ASSERT_FLOAT_EQ(a & b, 15.5f);

	c = a ^ b;

	ASSERT_FLOAT_EQ(c.x, -12.f);
	ASSERT_FLOAT_EQ(c.y, 5.25f);
	ASSERT_FLOAT_EQ(c.z, -1.5f);

	c = a;

	ASSERT_EQ(a.cmpeq(b), 0x0);
	ASSERT_EQ(a.cmpeq(c), 0x7);
	ASSERT_EQ(a.cmpeq(c), c.cmpeq(a));
	ASSERT_EQ(a.cmpne(b), 0x7);
	ASSERT_EQ(a.cmpne(c), 0x0);
	ASSERT_EQ(a.cmpne(c), c.cmpne(a));
	ASSERT_EQ(a.cmplt(b), 0x3);
	ASSERT_EQ(a.cmplt(c), 0x0);
	ASSERT_EQ(a.cmpgt(b), 0x4);
	ASSERT_EQ(a.cmpgt(c), 0x0);
	ASSERT_EQ(a.cmple(b), 0x3);
	ASSERT_EQ(a.cmple(c), 0x7);
	ASSERT_EQ(a.cmpge(b), 0x4);
	ASSERT_EQ(a.cmpge(c), 0x7);

	Vec3<int32> point = a;
	
	ASSERT_EQ(point.x, 0);
	ASSERT_EQ(point.y, 2);
	ASSERT_EQ(point.z, 3);

	SUCCEED();
}

TEST(math, vec4)
{
	Vec4<float32> a, b, c, d;

	a = Vec4<float32>{0.4f, 0.3f, 0.f, 1.f};

	ASSERT_FLOAT_EQ(a.getSquaredSize(), 0.4f * 0.4f + 0.3f * 0.3f + 1.f);
	ASSERT_FLOAT_EQ(a.getSize(), PlatformMath::sqrt(a.getSquaredSize()));
	ASSERT_FLOAT_EQ((-a).getSquaredSize(), a.getSquaredSize());

	b = a / a.getSize();
	
	ASSERT_FLOAT_EQ(b.getSquaredSize(), 1.f);

	ASSERT_FLOAT_EQ(a.getNormal().getSquaredSize(), 1.f);

	a.normalize();

	ASSERT_FLOAT_EQ(a.getSquaredSize(), 1.f);

	a = Vec4<float32>{0.4f, 0.3f, 0.f, 1.f};
	b = Vec4<float32>{0.8f, 0.6f, 0.2f, 0.9f};
	b += a;

	ASSERT_FLOAT_EQ(b.x, 1.2f);
	ASSERT_FLOAT_EQ(b.y, 0.9f);
	ASSERT_FLOAT_EQ(b.z, 0.2f);
	ASSERT_FLOAT_EQ(b.w, 1.9f);

	c = Vec4<float32>{0.5, 2.f, 1.f, 0.f};
	b *= c;

	ASSERT_FLOAT_EQ(b.x, 0.6f);
	ASSERT_FLOAT_EQ(b.y, 1.8f);
	ASSERT_FLOAT_EQ(b.z, 0.2f);
	ASSERT_FLOAT_EQ(b.w, 0.f);

	b.x = 0.6f;
	b.y = 1.8f;
	b.z = 0.f;
	b.w = 2.f;
	b -= c;

	ASSERT_FLOAT_EQ(b.x, 0.1f);
	ASSERT_FLOAT_EQ(b.y, -0.2f);
	ASSERT_FLOAT_EQ(b.z, -1.f);
	ASSERT_FLOAT_EQ(b.w, 2.f);

	b /= b;

	ASSERT_FLOAT_EQ(b.x, 1.f);
	ASSERT_FLOAT_EQ(b.y, 1.f);
	ASSERT_FLOAT_EQ(b.z, 1.f);
	ASSERT_FLOAT_EQ(b.w, 1.f);

	a = Vec4<float32>{0.5f, 2.f, 3.f, 1.f};
	b = Vec4<float32>{2.f, 5.f, 1.5f, 1.f};

	ASSERT_FLOAT_EQ(a & b, 16.5f);

	c = a;

	ASSERT_EQ(a.cmpeq(b), 0x8);
	ASSERT_EQ(a.cmpeq(c), 0xf);
	ASSERT_EQ(a.cmpeq(c), c.cmpeq(a));
	ASSERT_EQ(a.cmpne(b), 0x7);
	ASSERT_EQ(a.cmpne(c), 0x0);
	ASSERT_EQ(a.cmpne(c), c.cmpne(a));
	ASSERT_EQ(a.cmplt(b), 0x3);
	ASSERT_EQ(a.cmplt(c), 0x0);
	ASSERT_EQ(a.cmpgt(b), 0x4);
	ASSERT_EQ(a.cmpgt(c), 0x0);
	ASSERT_EQ(a.cmple(b), 0xb);
	ASSERT_EQ(a.cmple(c), 0xf);
	ASSERT_EQ(a.cmpge(b), 0xc);
	ASSERT_EQ(a.cmpge(c), 0xf);

	Vec4<int32> point = a;
	
	ASSERT_EQ(point.x, 0);
	ASSERT_EQ(point.y, 2);
	ASSERT_EQ(point.z, 3);
	ASSERT_EQ(point.w, 1);

	SUCCEED();
}

TEST(math, quat)
{
	Quat r, s, t;
	Vec3<float32> a, b, c;

	r = {M_PI_2, Vec3<float32>{0.f, 1.f, 0.f}};
	s = {M_PI_4, Vec3<float32>{0.f, 1.f, 0.f}};

	a = {1.f, 0.f, 0.f};
	b = {0.f, 0.f, -1.f};
	c = {1.f, 0.f, -1.f};
	c.normalize();

	ASSERT_TRUE((b - (r * a)).isNearlyZero());
	ASSERT_TRUE((c - (s * a)).isNearlyZero());

	t = r * s;
	c = {-1.f, 0.f, -1.f};
	c.normalize();

	ASSERT_TRUE(((t * a) - (r * (s * a))).isNearlyZero());
	ASSERT_TRUE((c - (t * a)).isNearlyZero());

	SUCCEED();
}