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

    UniformGrid grid_;

    IntersectionFlags intersections_;
    Renderer renderer_;
};

} // namespace tri3d
