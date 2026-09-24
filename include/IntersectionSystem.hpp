#pragma once

#include <cstdint>
#include <vector>

namespace tri3d {

class Scene;
class UniformGrid;

// По одному флагу на Triangle в Scene:
// 0 — ни с кем не пересекается;
// 1 — пересекается хотя бы с одним другим треугольником.
using IntersectionFlags = std::vector<std::uint8_t>;

class IntersectionSystem
{
public:
    // Для каждой непустой ячейки UniformGrid перебирает все пары
    // содержащихся в ней треугольников и выполняет точную проверку пересечения.
    //
    // На первом этапе одинаковая пара, попавшая сразу в несколько ячеек,
    // может быть проверена повторно. Это не влияет на корректность результата;
    // дедупликацию добавим только если профилирование покажет необходимость.
    void calculate(
        const Scene& scene,
        const UniformGrid& grid,
        IntersectionFlags& results
    ) const;
};

} // namespace tri3d
