#pragma once

#include "Camera.hpp"
#include "IntersectionSystem.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "UniformGrid.hpp"

namespace tri3d {

// Верхнеуровневый координатор программы.
class Engine
{
public:
    void run();

private:
    Scene scene_;
    Camera camera_;

    // Для нашего целевого сценария начинаем с cell size = 1.
    UniformGrid grid_{1.0f};

    IntersectionSystem intersectionSystem_;
    IntersectionFlags intersections_;
    Renderer renderer_;
};

} // namespace tri3d
