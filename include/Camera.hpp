#pragma once

#include "Vec3.hpp"

namespace tri3d {

class Camera
{
public:
    // Создаёт перспективную камеру.
    //
    // position:
    //     Положение камеры в мировых координатах.
    //
    // target:
    //     Точка в мировых координатах, в которую камера смотрит
    //     в момент создания.
    //
    // worldUp:
    //     Направление "вверх" в мировом пространстве.
    //     Обычно:
    //
    //         { 0.0f, 1.0f, 0.0f }
    //
    //     Не обязано быть нормализовано.
    //
    // verticalFovRadians:
    //     Вертикальный угол обзора в РАДИАНАХ.
    //
    //     Должен удовлетворять:
    //
    //         0 < verticalFovRadians < pi
    //
    // nearPlane:
    //     Расстояние до ближней плоскости отсечения.
    //
    //     Должно быть:
    //
    //         nearPlane > 0
    //
    // farPlane:
    //     Расстояние до дальней плоскости отсечения.
    //
    //     Должно быть:
    //
    //         farPlane > nearPlane
    //
    // Инварианты:
    //
    //     position != target
    //
    //     worldUp не должен быть нулевым вектором.
    //
    //     Направление взгляда не должно быть параллельно worldUp,
    //     иначе невозможно построить корректный ортонормированный
    //     базис камеры.
    //
    // При некорректных параметрах constructor должен бросать
    // std::invalid_argument.
    Camera(
        const Vec3& position,
        const Vec3& target,
        const Vec3& worldUp,
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );


    // ========================================================
    // Состояние, необходимое Renderer
    // ========================================================

    // Текущее положение камеры в мировом пространстве.
    [[nodiscard]]
    const Vec3& position() const noexcept;


    // Нормализованное направление взгляда камеры.
    //
    // Всегда имеет длину 1.
    [[nodiscard]]
    const Vec3& forward() const noexcept;


    // Нормализованная локальная ось камеры "вправо".
    //
    // Перпендикулярна forward() и up().
    // Всегда имеет длину 1.
    [[nodiscard]]
    const Vec3& right() const noexcept;


    // Нормализованная локальная ось камеры "вверх".
    //
    // Это НЕ обязательно исходный worldUp.
    //
    // После построения ортонормированного базиса up()
    // должен быть точно перпендикулярен forward() и right().
    [[nodiscard]]
    const Vec3& up() const noexcept;


    // Вертикальный угол обзора камеры в радианах.
    //
    // Renderer использует его при построении
    // perspective projection.
    [[nodiscard]]
    float verticalFov() const noexcept;


    // Расстояние до ближней плоскости отсечения.
    [[nodiscard]]
    float nearPlane() const noexcept;


    // Расстояние до дальней плоскости отсечения.
    [[nodiscard]]
    float farPlane() const noexcept;


    // ========================================================
    // Управление положением камеры
    // ========================================================

    // Перемещает камеру вдоль направления её взгляда.
    //
    // distance > 0:
    //     движение вперёд.
    //
    // distance < 0:
    //     движение назад.
    //
    // Ориентация камеры не изменяется.
    void moveForward(float distance) noexcept;


    // Перемещает камеру вдоль локальной оси right().
    //
    // distance > 0:
    //     движение вправо.
    //
    // distance < 0:
    //     движение влево.
    //
    // Ориентация камеры не изменяется.
    void moveRight(float distance) noexcept;


    // Перемещает камеру вдоль локальной оси up().
    //
    // distance > 0:
    //     движение вверх относительно камеры.
    //
    // distance < 0:
    //     движение вниз относительно камеры.
    //
    // Это именно LOCAL up камеры, а не обязательно worldUp.
    void moveUp(float distance) noexcept;


    // Устанавливает положение камеры напрямую.
    //
    // Ориентация камеры не изменяется.
    void setPosition(const Vec3& position) noexcept;


    // ========================================================
    // Управление ориентацией
    // ========================================================

    // Поворачивает камеру.
    //
    // Оба угла задаются в РАДИАНАХ.
    //
    // yawRadians:
    //     Поворот влево/вправо вокруг worldUp.
    //
    // pitchRadians:
    //     Поворот вверх/вниз вокруг текущей локальной
    //     оси right().
    //
    // После поворота необходимо заново построить
    // ортонормированный базис:
    //
    //     forward
    //     right
    //     up
    //
    // Реализация НЕ должна позволять forward стать
    // параллельным worldUp, иначе right/up перестанут
    // существовать.
    //
    // Конкретный способ ограничения pitch является
    // внутренней деталью Camera.
    void rotate(
        float yawRadians,
        float pitchRadians
    );


    // Направляет камеру на конкретную точку мира.
    //
    // position камеры не изменяется.
    //
    // После вызова:
    //
    //     forward =
    //         normalize(target - position)
    //
    // После этого должен быть заново построен
    // ортонормированный базис камеры.
    //
    // target не должен совпадать с position.
    //
    // Если новое направление оказывается недопустимо
    // относительно worldUp, функция должна бросить
    // std::invalid_argument.
    void lookAt(const Vec3& target);


    // ========================================================
    // Perspective parameters
    // ========================================================

    // Изменяет параметры перспективной камеры.
    //
    // verticalFovRadians:
    //
    //     0 < verticalFovRadians < pi
    //
    // nearPlane:
    //
    //     nearPlane > 0
    //
    // farPlane:
    //
    //     farPlane > nearPlane
    //
    // При некорректных параметрах функция должна бросить
    // std::invalid_argument.
    //
    // Aspect ratio здесь намеренно отсутствует.
    //
    // Размер viewport/window относится к Renderer,
    // поэтому Renderer сам вычисляет:
    //
    //     aspectRatio =
    //         static_cast<float>(width) / height;
    //
    void setPerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );


private:
    // Положение камеры в мировом пространстве.
    Vec3 position_;


    // Ортонормированный базис камеры.
    //
    // Все три вектора должны быть нормализованы
    // и взаимно перпендикулярны.
    Vec3 forward_;
    Vec3 right_;
    Vec3 up_;


    // Постоянное понятие "верх мира".
    //
    // Используется при yaw и при восстановлении
    // базиса камеры.
    //
    // Хранить нормализованным.
    Vec3 worldUp_;


    // Параметры perspective projection.
    float verticalFovRadians_;
    float nearPlane_;
    float farPlane_;


    // Перестраивает right_ и up_ на основе
    // текущих forward_ и worldUp_.
    //
    // После выполнения должно выполняться:
    //
    //     length(forward_) == 1
    //     length(right_)   == 1
    //     length(up_)      == 1
    //
    // и все оси должны быть взаимно перпендикулярны.
    //
    // Конкретный порядок cross зависит от принятой
    // системы координат и должен быть единообразным
    // во всём проекте.
    void rebuildBasis();


    // Проверяет параметры perspective projection.
    //
    // При нарушении инвариантов бросает
    // std::invalid_argument.
    static void validatePerspective(
        float verticalFovRadians,
        float nearPlane,
        float farPlane
    );
};

} // namespace tri3ds