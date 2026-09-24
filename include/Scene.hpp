#pragma once

#include <vector>

#include "Triangle.hpp"
#include "Vec3.hpp"

namespace tri3d {

struct Scene
{
    std::vector<Triangle> triangles;

    Vec3 min;
    Vec3 max;
};

} // namespace tri3d