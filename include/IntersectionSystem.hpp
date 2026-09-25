#pragma once

#include <cstdint>
#include <vector>

#include "Scene.hpp"
#include "UniformGrid.hpp"

namespace tri3d {

using IntersectionFlags = std::vector<std::uint8_t>;

[[nodiscard]]
IntersectionFlags findIntersections(
    const Scene& scene,
    const UniformGrid& grid
);

} // namespace tri3ds