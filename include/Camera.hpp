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

    // Создаёт перспективную Camera.
    //
    // position:
    //     Положение камеры в world-space.
    //
    // target:
    //     Точка мира, на которую смотрит Camera.
    //
    // Обязательно:
    //
    //     target != position
    //
    // worldUp:
    //     Глобальное направление "вверх".
    //
    // Обычно:
    //
    //     { 0, 1, 0 }
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
    // Все числовые значения должны быть finite.
    //
    // При нарушении инвариантов constructor бросает:
    //
    //     std::invalid_argument
    //
    // После создания должен существовать корректный
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

    // Текущее положение Camera в world-space.
    [[nodiscard]]
    const Vec3& position() const noexcept;


    // Нормализованный WORLD-SPACE вектор,
    // направленный туда, куда Camera смотрит.
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


    // Нормализованная локальная ось Camera вправо.
    //
    // Всегда перпендикулярна:
    //
    //     forward()
    //     up()
    //
    // с учётом погрешности float.
    [[nodiscard]]
    const Vec3& right() const noexcept;


    // Нормализованная локальная ось Camera вверх.
    //
    // Это НЕ обязательно исходный worldUp.
    //
    // worldUp определяет глобальное понятие "верх",
    // а up является частью текущего ортонормированного
    // базиса Camera.
    [[nodiscard]]
    const Vec3& up() const noexcept;


    // Вертикальный FOV в РАДИАНАХ.
    [[nodiscard]]
    float verticalFovRadians() const noexcept;


    // Расстояние до near clipping plane.
    [[nodiscard]]
    float nearPlane() const noexcept;


    // Расстояние до far clipping plane.
    [[nodiscard]]
    float farPlane() const noexcept;


    // ========================================================
    // Matrices for Renderer
    // ========================================================

    // Возвращает View Matrix текущей Camera.
    //
    // Результат должен быть эквивалентен:
    //
    //     makeViewMatrix(
    //         position(),
    //         right(),
    //         up(),
    //         forward()
    //     );
    //
    // Renderer НЕ должен самостоятельно строить
    // View Matrix из внутренностей Camera.
    //
    // В view-space Camera смотрит вдоль:
    //
    //     -Z
    //
    [[nodiscard]]
    Mat4 viewMatrix() const noexcept;


    // Возвращает Vulkan-compatible
    // Perspective Projection Matrix.
    //
    // aspectRatio передаёт Renderer:
    //
    //     static_cast<float>(width) /
    //     static_cast<float>(height)
    //
    // Camera намеренно НЕ хранит:
    //
    //     window width
    //     window height
    //     aspect ratio
    //
    // потому что они принадлежат Renderer/viewport.
    //
    // Результат должен быть эквивалентен:
    //
    //     makePerspectiveMatrix(
    //         verticalFovRadians(),
    //         aspectRatio,
    //         nearPlane(),
    //         farPlane()
    //     );
    //
    // После perspective divide используется Vulkan NDC:
    //
    //     X: [-1, +1]
    //     Y: [-1, +1]
    //     Z: [ 0, +1]
    //
    // near plane:
    //
    //     z = 0
    //
    // far plane:
    //
    //     z = 1
    //
    // Camera НЕ переворачивает Y.
    //
    // Renderer делает это через отрицательный
    // VkViewport::height.
    //
    // При некорректном aspectRatio бросает:
    //
    //     std::invalid_argument
    //
    [[nodiscard]]
    Mat4 projectionMatrix(
        float aspectRatio
    ) const;


    // ========================================================
    // Position control
    // ========================================================

    // Перемещает Camera вдоль её forward().
    //
    // distance > 0:
    //     движение вперёд.
    //
    // distance < 0:
    //     движение назад.
    //
    // Ориентация Camera не изменяется.
    void moveForward(
        float distance
    ) noexcept;


    // Перемещает Camera вдоль её локальной right().
    //
    // distance > 0:
    //     вправо.
    //
    // distance < 0:
    //     влево.
    //
    // Ориентация Camera не изменяется.
    void moveRight(
        float distance
    ) noexcept;


    // Перемещает Camera вдоль её ЛОКАЛЬНОЙ up().
    //
    // distance > 0:
    //     вверх.
    //
    // distance < 0:
    //     вниз.
    //
    // Это именно:
    //
    //     camera.up()
    //
    // а не обязательно worldUp_.
    //
    // Ориентация Camera не изменяется.
    void moveUp(
        float distance
    ) noexcept;


    // Устанавливает положение Camera напрямую.
    //
    // Ориентация Camera не изменяется.
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
    //
    //     Поворот вокруг worldUp_.
    //
    //     Положительное направление соответствует
    //     правилу правой руки вокруг worldUp_.
    //
    // pitchRadians:
    //
    //     Поворот вверх/вниз относительно локальной
    //     оси right_.
    //
    //     При стандартном:
    //
    //         worldUp = { 0, 1, 0 }
    //
    //     положительный pitch должен направлять
    //     взгляд вверх.
    //
    // После поворота нужно восстановить корректный
    // ортонормированный базис:
    //
    //     forward_
    //     right_
    //     up_
    //
    // Camera никогда не должна попасть в состояние,
    // где:
    //
    //     forward_ || worldUp_
    //
    // потому что тогда невозможно определить right_.
    //
    // Реализация должна ограничивать pitch,
    // чтобы Camera не достигала такого состояния.
    //
    // Точное значение ограничения является
    // внутренней деталью реализации.
    void rotate(
        float yawRadians,
        float pitchRadians
    );


    // Направляет Camera на конкретную world-space точку.
    //
    // position_ не изменяется.
    //
    // После выполнения:
    //
    //     forward_ =
    //         normalize(target - position_)
    //
    // После этого необходимо перестроить:
    //
    //     right_
    //     up_
    //
    // Некорректные случаи:
    //
    //     target == position_
    //
    // либо:
    //
    //     новое направление параллельно worldUp_
    //
    // В этих случаях бросает:
    //
    //     std::invalid_argument
    //
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
    //     verticalFovRadians finite
    //
    //     0 < verticalFovRadians < pi
    //
    //     nearPlane finite
    //
    //     nearPlane > 0
    //
    //     farPlane finite
    //
    //     farPlane > nearPlane
    //
    // При нарушении требований бросает:
    //
    //     std::invalid_argument
    //
    // Aspect ratio здесь отсутствует намеренно:
    //
    // он определяется текущим viewport Renderer.
    //
    void setPerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );


private:

    // ========================================================
    // Transform
    // ========================================================

    // Положение в world-space.
    Vec3 position_;


    // WORLD-SPACE направление взгляда.
    //
    // Всегда нормализовано.
    Vec3 forward_;


    // Локальная ось Camera вправо.
    //
    // Всегда нормализована.
    Vec3 right_;


    // Локальная ось Camera вверх.
    //
    // Всегда нормализована.
    Vec3 up_;


    // ========================================================
    // Global orientation reference
    // ========================================================

    // Глобальное направление "вверх".
    //
    // После constructor должно храниться
    // в нормализованном виде.
    //
    // Стандартное значение:
    //
    //     { 0, 1, 0 }
    //
    Vec3 worldUp_;


    // ========================================================
    // Perspective parameters
    // ========================================================

    float verticalFovRadians_;

    float nearPlane_;

    float farPlane_;


    // ========================================================
    // Internal helpers
    // ========================================================

    // Перестраивает right_ и up_ на основе:
    //
    //     forward_
    //     worldUp_
    //
    // Для принятой нами правосторонней системы:
    //
    //     right_ =
    //         normalize(
    //             cross(forward_, worldUp_)
    //         );
    //
    //     up_ =
    //         cross(right_, forward_);
    //
    // После выполнения должны выполняться:
    //
    //     length(forward_) ~= 1
    //
    //     length(right_) ~= 1
    //
    //     length(up_) ~= 1
    //
    // и:
    //
    //     dot(forward_, right_) ~= 0
    //
    //     dot(forward_, up_) ~= 0
    //
    //     dot(right_, up_) ~= 0
    //
    // Здесь "~=" означает допустимую погрешность float,
    // а не точное сравнение.
    //
    void rebuildBasis();


    // Проверяет параметры perspective projection.
    //
    // Должно проверяться:
    //
    //     finite(verticalFovRadians)
    //
    //     0 < verticalFovRadians < pi
    //
    //     finite(nearPlane)
    //
    //     nearPlane > 0
    //
    //     finite(farPlane)
    //
    //     farPlane > nearPlane
    //
    // При ошибке бросает:
    //
    //     std::invalid_argument
    //
    static void validatePerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );
};

} // namespace tri3d