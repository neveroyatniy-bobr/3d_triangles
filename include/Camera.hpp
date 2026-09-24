#pragma once

#include "Vec3.hpp"

namespace tri3d {

// Камера является классом, потому что у ее состояния есть инварианты,
// а движение должно выражаться семантическими операциями.
class Camera
{
public:
    Camera() = default;

    // Положение и ориентация.
    void setPosition(Vec3 position);
    void lookAt(Vec3 target, Vec3 up = {0.0f, 1.0f, 0.0f});

    // Параметры перспективной камеры.
    // Реализация должна будет контролировать корректность FOV и плоскостей отсечения.
    void setPerspective(
        float verticalFovDegrees,
        float nearPlane,
        float farPlane
    );

    // Семантическое движение относительно ориентации камеры.
    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);

    // Поворот камеры. Конкретные ограничения будут определены при реализации.
    void rotate(float yawDegrees, float pitchDegrees);

    [[nodiscard]] const Vec3& position() const noexcept;
    [[nodiscard]] const Vec3& target() const noexcept;
    [[nodiscard]] const Vec3& up() const noexcept;

    [[nodiscard]] float verticalFovDegrees() const noexcept;
    [[nodiscard]] float nearPlane() const noexcept;
    [[nodiscard]] float farPlane() const noexcept;

private:
    Vec3 position_{0.0f, 0.0f, 5.0f};
    Vec3 target_{0.0f, 0.0f, 0.0f};
    Vec3 up_{0.0f, 1.0f, 0.0f};

    float verticalFovDegrees_ = 60.0f;
    float nearPlane_ = 0.1f;
    float farPlane_ = 1000.0f;
};

} // namespace tri3d
