#include "IntersectionSystem.hpp"

namespace tri3d {

IntersectionFlags findIntersections(
    const Scene& scene,
    const UniformGrid& grid
)
{
    IntersectionFlags flags(
        scene.triangles.size(),
        0
    );

    for (const auto& cell : grid.cells())
    {
        for (std::size_t i = 0; i < cell.size(); ++i)
        {
            for (std::size_t j = i + 1; j < cell.size(); ++j)
            {
                const auto firstIndex = cell[i];
                const auto secondIndex = cell[j];

                if (flags[firstIndex] && flags[secondIndex])
                    continue;

                if (intersects(
                    scene.triangles[firstIndex],
                    scene.triangles[secondIndex]
                ))
                {
                    flags[firstIndex] = 1;
                    flags[secondIndex] = 1;
                }
            }
        }
    }

    return flags;
}

} // namespace tri3d