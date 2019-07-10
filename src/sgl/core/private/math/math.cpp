#include "math/math.h"

//////////////////////////////////////////////////
// Vec3
//////////////////////////////////////////////////

template<> const Vec3<float32> Vec3<float32>::zero = {0.f};
template<> const Vec3<float32> Vec3<float32>::unit = {1.f};
template<> const Vec3<float32> Vec3<float32>::right = {1.f, 0.f, 0.f};
template<> const Vec3<float32> Vec3<float32>::left = {-1.f, 0.f, 0.f};
template<> const Vec3<float32> Vec3<float32>::up = {0.f, 1.f, 0.f};
template<> const Vec3<float32> Vec3<float32>::down = {0.f, -1.f, 0.f};
template<> const Vec3<float32> Vec3<float32>::forward = {0.f, 0.f, 1.f};
template<> const Vec3<float32> Vec3<float32>::backward = {0.f, 0.f, -1.f};