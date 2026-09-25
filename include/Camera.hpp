#pragma once

#include "Mat4.hpp"
#include "Vec3.hpp"

namespace tri3d {


class Camera
{
public:

    // ========================================================
    // Construction
    // ========================================================

    // Создаёт перспективную камеру.
    //
    // position:
    //     Положение камеры в мировых координатах.
    //
    // target:
    //     Точка, на которую камера смотрит.
    //
    //     Обязательное условие:
    //
    //         target != position
    //
    // worldUp:
    //     Направление "вверх" для мира.
    //
    // Обычно:
    //
    //         { 0, 1, 0 }
    //
    // Может быть ненормализованным.
    //
    // Не должен быть нулевым.
    //
    // Направление:
    //
    //     target - position
    //
    // не должно быть параллельно worldUp.
    //
    // verticalFovRadians:
    //     Вертикальный FOV в РАДИАНАХ.
    //
    // nearPlane:
    //     Расстояние до near clipping plane.
    //
    // farPlane:
    //     Расстояние до far clipping plane.
    //
    // Требования:
    //
    //     0 < verticalFovRadians < pi
    //
    //     nearPlane > 0
    //
    //     farPlane > nearPlane
    //
    // Все числовые параметры должны быть finite.
    //
    // При нарушении инвариантов constructor должен бросать:
    //
    //     std::invalid_argument
    //
    // После создания Camera обязана иметь корректный
    // ортонормированный базис:
    //
    //     forward_
    //     right_
    //     up_
    //
    Camera(
        const Vec3& position,
        const Vec3& target,
        const Vec3& worldUp,
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );


    // ========================================================
    // Camera state
    // ========================================================

    // Текущее положение камеры в world-space.
    [[nodiscard]]
    const Vec3& position() const noexcept;


    // Нормализованный вектор, направленный туда,
    // куда камера смотрит.
    //
    // Это WORLD-SPACE направление.
    //
    // Например:
    //
    //     position = { 0, 0, 10 }
    //     target   = { 0, 0, 0 }
    //
    // значит:
    //
    //     forward = { 0, 0, -1 }
    //
    // Всегда имеет длину 1.
    [[nodiscard]]
    const Vec3& forward() const noexcept;


    // Нормализованная локальная ось камеры вправо.
    //
    // Всегда:
    //
    //     dot(right, forward) == 0
    //
    //     dot(right, up) == 0
    //
    // с учётом обычной погрешности float.
    [[nodiscard]]
    const Vec3& right() const noexcept;


    // Нормализованная локальная ось камеры вверх.
    //
    // Это не обязательно ровно worldUp.
    //
    // worldUp задаёт глобальное понятие верха,
    // а up является частью текущего ортонормированного
    // базиса камеры.
    [[nodiscard]]
    const Vec3& up() const noexcept;


    // Вертикальный FOV в радианах.
    [[nodiscard]]
    float verticalFovRadians() const noexcept;


    // Расстояние до near plane.
    [[nodiscard]]
    float nearPlane() const noexcept;


    // Расстояние до far plane.
    [[nodiscard]]
    float farPlane() const noexcept;


    // ========================================================
    // Matrices for Renderer
    // ========================================================

    // Возвращает View Matrix для текущего состояния Camera.
    //
    // Должна быть эквивалентна:
    //
    //     makeViewMatrix(
    //         position(),
    //         right(),
    //         up(),
    //         forward()
    //     );
    //
    // Renderer не должен самостоятельно знать,
    // как строится View Matrix.
    [[nodiscard]]
    Mat4 viewMatrix() const noexcept;


    // Возвращает Perspective Projection Matrix.
    //
    // aspectRatio передаёт Renderer:
    //
    //     static_cast<float>(width) / height
    //
    // Camera намеренно НЕ хранит размеры окна
    // и aspect ratio.
    //
    // Должна быть эквивалентна:
    //
    //     makePerspectiveMatrix(
    //         verticalFovRadians(),
    //         aspectRatio,
    //         nearPlane(),
    //         farPlane()
    //     );
    //
    // Если aspectRatio некорректен,
    // должна бросить std::invalid_argument.
    [[nodiscard]]
    Mat4 projectionMatrix(
        float aspectRatio
    ) const;


    // ========================================================
    // Position control
    // ========================================================

    // Перемещает Camera вдоль локального forward.
    //
    // distance > 0:
    //     движение вперёд.
    //
    // distance < 0:
    //     движение назад.
    //
    // Ориентация камеры не изменяется.
    void moveForward(float distance) noexcept;


    // Перемещает Camera вдоль локальной оси right.
    //
    // distance > 0:
    //     вправо.
    //
    // distance < 0:
    //     влево.
    //
    // Ориентация не изменяется.
    void moveRight(float distance) noexcept;


    // Перемещает Camera вдоль её ЛОКАЛЬНОЙ оси up.
    //
    // distance > 0:
    //     вверх.
    //
    // distance < 0:
    //     вниз.
    //
    // Важно:
    //
    // это camera.up(), а не обязательно worldUp_.
    void moveUp(float distance) noexcept;


    // Напрямую устанавливает world-space position.
    //
    // Ориентация камеры не меняется.
    void setPosition(
        const Vec3& position
    ) noexcept;


    // ========================================================
    // Orientation control
    // ========================================================

    // Поворачивает Camera относительно текущего направления.
    //
    // Все углы задаются в РАДИАНАХ.
    //
    // yawRadians:
    //     Поворот вокруг worldUp_.
    //
    //     Положительное направление соответствует
    //     правилу правой руки вокруг worldUp_.
    //
    // pitchRadians:
    //     Поворот вокруг текущей локальной right_.
    //
    //     Положительный pitch при стандартном:
    //
    //         worldUp = { 0, 1, 0 }
    //
    //     должен направлять взгляд вверх.
    //
    // После поворота необходимо восстановить
    // корректный ортонормированный базис:
    //
    //     forward_
    //     right_
    //     up_
    //
    // Camera никогда не должна оказываться в состоянии,
    // где forward_ параллелен worldUp_.
    //
    // Реализация может ограничивать pitch так,
    // чтобы камера не достигала полюса.
    //
    // Конкретная величина ограничения является
    // внутренней деталью реализации.
    void rotate(
        float yawRadians,
        float pitchRadians
    );


    // Направляет Camera на world-space точку target.
    //
    // position не изменяется.
    //
    // После выполнения:
    //
    //     forward =
    //         normalize(target - position)
    //
    // Затем необходимо перестроить right/up.
    //
    // Некорректно:
    //
    //     target == position
    //
    // а также направление взгляда,
    // параллельное worldUp_.
    //
    // При некорректном target бросает:
    //
    //     std::invalid_argument
    void lookAt(
        const Vec3& target
    );


    // ========================================================
    // Perspective configuration
    // ========================================================

    // Изменяет параметры perspective projection.
    //
    // Требования:
    //
    //     0 < verticalFovRadians < pi
    //
    //     nearPlane > 0
    //
    //     farPlane > nearPlane
    //
    // Все значения должны быть finite.
    //
    // При ошибке бросает:
    //
    //     std::invalid_argument
    //
    // Aspect ratio здесь отсутствует намеренно:
    //
    // он определяется размером viewport Renderer.
    void setPerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );


private:

    // ========================================================
    // Transform
    // ========================================================

    Vec3 position_;

    // WORLD-SPACE направление взгляда.
    //
    // Всегда нормализовано.
    Vec3 forward_;

    // Ортонормированные локальные оси Camera.
    Vec3 right_;
    Vec3 up_;


    // ========================================================
    // Global orientation reference
    // ========================================================

    // Глобальное понятие "верх".
    //
    // После constructor хранится нормализованным.
    //
    // Например:
    //
    //     { 0, 1, 0 }
    Vec3 worldUp_;


    // ========================================================
    // Perspective
    // ========================================================

    float verticalFovRadians_;
    float nearPlane_;
    float farPlane_;


    // ========================================================
    // Internal helpers
    // ========================================================

    // Перестраивает right_ и up_ из forward_ и worldUp_.
    //
    // Для нашей правосторонней системы координат:
    //
    //     right =
    //         normalize(cross(forward, worldUp))
    //
    //     up =
    //         cross(right, forward)
    //
    // После выполнения должны выполняться:
    //
    //     length(forward_) == 1
    //     length(right_)   == 1
    //     length(up_)      == 1
    //
    // и:
    //
    //     dot(forward_, right_) == 0
    //     dot(forward_, up_)    == 0
    //     dot(right_, up_)      == 0
    //
    // с допустимой погрешностью float.
    void rebuildBasis();


    // Проверяет параметры perspective projection.
    //
    // При нарушении требований бросает:
    //
    //     std::invalid_argument
    static void validatePerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );
};

} // namespace tri3ds