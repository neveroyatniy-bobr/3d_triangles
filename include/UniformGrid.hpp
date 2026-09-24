#pragma once

#include <cstddef>
#include <vector>

#include "Scene.hpp"

namespace tri3d {

class UniformGrid
{
public:
    using TriangleIndex = std::size_t;
    using Cell = std::vector<TriangleIndex>;

    // Строит сетку для всей сцены с заданным размером клетки.
    UniformGrid(
        const Scene& scene,
        float cellSize
    );

    // Read-only доступ ко всем клеткам сетки.
    // Копирования cells_ не происходит.
    [[nodiscard]]
    const std::vector<Cell>& cells() const noexcept;

private:
    float cellSize_;

    // Начало сетки в мировых координатах.
    Vec3 origin_;

    std::size_t sizeX_ = 0;
    std::size_t sizeY_ = 0;
    std::size_t sizeZ_ = 0;

    // TODO:
    // Позже заменить vector<Cell> на более компактное хранение:
    // один общий массив TriangleIndex + offsets клеток.
    //
    // TODO:
    // Позже добавить конфигурацию UniformGrid:
    // Manual mode -> cellSize задаётся вручную.
    // Auto mode   -> cellSize вычисляется по характеристикам Scene
    //                (количество Triangle, объём сцены,
    //                 статистика размеров AABB и т.д.).
    std::vector<Cell> cells_;

    [[nodiscard]]
    std::size_t calculateAxisSize(
        float min,
        float max
    ) const;

    [[nodiscard]]
    std::size_t cellCoordinate(
        float value,
        float origin
    ) const;

    [[nodiscard]]
    std::size_t calculateCellCount() const;

    [[nodiscard]]
    std::size_t linearIndex(
        std::size_t x,
        std::size_t y,
        std::size_t z
    ) const noexcept;
};

} // namespace tri3d