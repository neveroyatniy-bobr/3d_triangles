#pragma once

#include "IntersectionSystem.hpp"

namespace tri3d {

class Camera;
class Scene;

// Отвечает только за отображение текущего состояния сцены.
class Renderer
{
public:
    void render(
        const Scene& scene,
        const Camera& camera,
        const IntersectionFlags& intersections
    );
};

} // namespace tri3d
