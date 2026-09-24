#include "UniformGrid.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace tri3d {

UniformGrid::UniformGrid(
    const Scene& scene,
    float cellSize
)
    : cellSize_(cellSize)
{
    // cellSize является собственным инвариантом UniformGrid.
    if (!std::isfinite(cellSize_) || cellSize_ <= 0.0f)
    {
        throw std::invalid_argument(
            "UniformGrid cell size must be finite and positive"
        );
    }

    // Пустая сцена -> пустая сетка.
    if (scene.triangles.empty())
    {
        return;
    }

    // Scene является простой структурой и сама свой
    // инвариант min <= max не защищает.
    if (scene.max.x < scene.min.x ||
        scene.max.y < scene.min.y ||
        scene.max.z < scene.min.z)
    {
        throw std::invalid_argument(
            "Invalid Scene bounds"
        );
    }

    origin_ = scene.min;

    sizeX_ = calculateAxisSize(
        scene.min.x,
        scene.max.x
    );

    sizeY_ = calculateAxisSize(
        scene.min.y,
        scene.max.y
    );

    sizeZ_ = calculateAxisSize(
        scene.min.z,
        scene.max.z
    );

    cells_.resize(calculateCellCount());


    for (
        std::size_t triangleIndex = 0;
        triangleIndex < scene.triangles.size();
        ++triangleIndex
    )
    {
        const Triangle& triangle =
            scene.triangles[triangleIndex];


        // Временный AABB.
        // После определения клеток он больше не нужен.
        const float minX = std::min({
            triangle.a.x,
            triangle.b.x,
            triangle.c.x
        });

        const float minY = std::min({
            triangle.a.y,
            triangle.b.y,
            triangle.c.y
        });

        const float minZ = std::min({
            triangle.a.z,
            triangle.b.z,
            triangle.c.z
        });

        const float maxX = std::max({
            triangle.a.x,
            triangle.b.x,
            triangle.c.x
        });

        const float maxY = std::max({
            triangle.a.y,
            triangle.b.y,
            triangle.c.y
        });

        const float maxZ = std::max({
            triangle.a.z,
            triangle.b.z,
            triangle.c.z
        });


        // Проверяем контракт Scene:
        // её min/max должны действительно содержать всю геометрию.
        if (minX < scene.min.x ||
            minY < scene.min.y ||
            minZ < scene.min.z ||
            maxX > scene.max.x ||
            maxY > scene.max.y ||
            maxZ > scene.max.z)
        {
            throw std::invalid_argument(
                "Scene bounds do not contain all triangles"
            );
        }


        const std::size_t minCellX =
            cellCoordinate(minX, origin_.x);

        const std::size_t minCellY =
            cellCoordinate(minY, origin_.y);

        const std::size_t minCellZ =
            cellCoordinate(minZ, origin_.z);

        const std::size_t maxCellX =
            cellCoordinate(maxX, origin_.x);

        const std::size_t maxCellY =
            cellCoordinate(maxY, origin_.y);

        const std::size_t maxCellZ =
            cellCoordinate(maxZ, origin_.z);


        // Треугольник записывается во все клетки,
        // затронутые его AABB.
        for (std::size_t z = minCellZ; z <= maxCellZ; ++z)
        {
            for (std::size_t y = minCellY; y <= maxCellY; ++y)
            {
                for (std::size_t x = minCellX; x <= maxCellX; ++x)
                {
                    cells_[linearIndex(x, y, z)]
                        .push_back(triangleIndex);
                }
            }
        }
    }
}


const std::vector<UniformGrid::Cell>&
UniformGrid::cells() const noexcept
{
    return cells_;
}


std::size_t UniformGrid::calculateAxisSize(
    float min,
    float max
) const
{
    // +1 намеренно:
    //
    // min = 0
    // max = 10
    // cellSize = 1
    //
    // получаем клетки 0 ... 10.
    return static_cast<std::size_t>(
        std::floor((max - min) / cellSize_)
    ) + 1;
}


std::size_t UniformGrid::cellCoordinate(
    float value,
    float origin
) const
{
    return static_cast<std::size_t>(
        std::floor((value - origin) / cellSize_)
    );
}


std::size_t UniformGrid::calculateCellCount() const
{
    // Защита от переполнения size_t при вычислении
    // sizeX_ * sizeY_ * sizeZ_.
    const std::size_t max =
        std::numeric_limits<std::size_t>::max();

    if (sizeX_ > max / sizeY_)
    {
        throw std::length_error(
            "UniformGrid is too large"
        );
    }

    const std::size_t sizeXY =
        sizeX_ * sizeY_;

    if (sizeXY > max / sizeZ_)
    {
        throw std::length_error(
            "UniformGrid is too large"
        );
    }

    return sizeXY * sizeZ_;
}


std::size_t UniformGrid::linearIndex(
    std::size_t x,
    std::size_t y,
    std::size_t z
) const noexcept
{
    return x + sizeX_ * (y + sizeY_ * z);
}

} // namespace tri3d