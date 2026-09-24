#pragma once

#include "Vec3.hpp"

namespace tri3d
{

struct Triangle
{
    Vec3 a;
    Vec3 b;
    Vec3 c;
};

[[nodiscard]]
bool intersects(
    const Triangle& first,
    const Triangle& second
) noexcept;

}