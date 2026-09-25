#pragma once

#include "Vec3.hpp"

namespace tri3d {


// ============================================================
// Vec4
// ============================================================

// Четырёхмерный вектор.
//
// Используется прежде всего для homogeneous coordinates
// в 3D rendering pipeline.
//
// 3D-точка:
//
//     { x, y, z, 1 }
//
// 3D-направление:
//
//     { x, y, z, 0 }
//
// После perspective projection значение w необходимо
// сохранить до выполнения perspective divide.
struct Vec4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};


// Арифметика Vec4 нужна Renderer в том числе
// для будущего clipping/interpolation.

[[nodiscard]]
Vec4 operator+(
    const Vec4& a,
    const Vec4& b
) noexcept;


[[nodiscard]]
Vec4 operator-(
    const Vec4& a,
    const Vec4& b
) noexcept;


[[nodiscard]]
Vec4 operator*(
    const Vec4& vector,
    float scalar
) noexcept;


[[nodiscard]]
Vec4 operator*(
    float scalar,
    const Vec4& vector
) noexcept;


// ============================================================
// Mat4
// ============================================================

// Матрица 4x4.
//
// ============================================================
// СОГЛАШЕНИЯ ДВИЖКА
// ============================================================
//
// 1. Хранение:
//
//        m[row][column]
//
// 2. Векторы считаются COLUMN vectors.
//
// 3. Преобразование вектора:
//
//        result = matrix * vector
//
// 4. Несколько преобразований:
//
//        clip = projection * view * world
//
//    То есть самая правая матрица применяется первой.
//
// 5. Используется правосторонняя система координат.
//
// 6. В VIEW SPACE камера смотрит вдоль -Z.
//
// 7. Используется Vulkan-compatible NDC:
//
//        X: [-1, +1]
//        Y: [-1, +1]
//        Z: [ 0, +1]
//
// 8. Y внутри projection matrix НЕ переворачивается.
//
//    Vulkan Renderer будет переворачивать viewport
//    при помощи отрицательного VkViewport::height.
//
// ============================================================
//
// Default initialization:
//
//     Mat4 matrix{};
//
// создаёт НУЛЕВУЮ матрицу.
//
// Для единичной матрицы использовать:
//
//     identityMatrix()
//
struct Mat4
{
    float m[4][4] = {};
};


// ============================================================
// Basic matrix operations
// ============================================================

// Возвращает единичную матрицу:
//
//     1 0 0 0
//     0 1 0 0
//     0 0 1 0
//     0 0 0 1
//
[[nodiscard]]
Mat4 identityMatrix() noexcept;


// Стандартное матричное умножение.
//
// Для:
//
//     Mat4 result = A * B;
//
// выражение:
//
//     result * vector
//
// сначала применяет B,
// затем A.
//
// Например:
//
//     projection * view
//
// означает:
//
//     world-space
//         ->
//     view-space
//         ->
//     clip-space
//
[[nodiscard]]
Mat4 operator*(
    const Mat4& left,
    const Mat4& right
) noexcept;


// Умножает Mat4 на homogeneous Vec4.
//
// Формально:
//
//     result[row] =
//         sum(
//             matrix[row][column] *
//             vector[column]
//         )
//
[[nodiscard]]
Vec4 operator*(
    const Mat4& matrix,
    const Vec4& vector
) noexcept;


// ============================================================
// Homogeneous coordinates
// ============================================================

// Преобразует Vec3-точку в homogeneous coordinates.
//
// Возвращает:
//
//     { x, y, z, 1 }
//
// Translation должен воздействовать на такую величину.
[[nodiscard]]
Vec4 pointToHomogeneous(
    const Vec3& point
) noexcept;


// Преобразует Vec3-направление в homogeneous coordinates.
//
// Возвращает:
//
//     { x, y, z, 0 }
//
// Translation НЕ должен воздействовать на direction.
[[nodiscard]]
Vec4 directionToHomogeneous(
    const Vec3& direction
) noexcept;


// Выполняет perspective divide:
//
//     x = vector.x / vector.w
//     y = vector.y / vector.w
//     z = vector.z / vector.w
//
// Возвращённый Vec3 находится в Vulkan NDC:
//
//     X: [-1, +1]
//     Y: [-1, +1]
//     Z: [ 0, +1]
//
// PRECONDITION:
//
//     vector.w != 0
//
// Функция намеренно noexcept и не делает runtime-проверку,
// потому что может вызываться для очень большого количества
// вершин.
//
// Renderer отвечает за корректный clipping и за то,
// что perspective divide выполняется только тогда,
// когда это допустимо.
[[nodiscard]]
Vec3 perspectiveDivide(
    const Vec4& vector
) noexcept;


// ============================================================
// View matrix
// ============================================================

// Создаёт View Matrix.
//
// position:
//     Положение камеры в world-space.
//
// right:
//     Нормализованная локальная ось камеры вправо.
//
// up:
//     Нормализованная локальная ось камеры вверх.
//
// forward:
//     Нормализованный world-space вектор,
//     направленный туда, куда камера смотрит.
//
// Camera гарантирует, что:
//
//     right
//     up
//     forward
//
// образуют корректный ортонормированный базис.
//
// ------------------------------------------------------------
//
// forward означает физическое направление взгляда.
//
// Например:
//
//     position = { 0, 0, 10 }
//     target   = { 0, 0, 0 }
//
// тогда:
//
//     forward = { 0, 0, -1 }
//
// При этом в VIEW SPACE камера всегда смотрит вдоль:
//
//     -Z
//
// ------------------------------------------------------------
//
// После:
//
//     view * pointToHomogeneous(position)
//
// положение камеры должно стать:
//
//     { 0, 0, 0, 1 }
//
// А точка перед камерой:
//
//     position + forward * distance
//
// после View Transform должна иметь:
//
//     z < 0
//
// если:
//
//     distance > 0
//
[[nodiscard]]
Mat4 makeViewMatrix(
    const Vec3& position,
    const Vec3& right,
    const Vec3& up,
    const Vec3& forward
) noexcept;


// ============================================================
// Perspective projection
// ============================================================

// Создаёт perspective projection matrix,
// совместимую с Vulkan.
//
// verticalFovRadians:
//     Вертикальный угол обзора в РАДИАНАХ.
//
// aspectRatio:
//     width / height viewport.
//
// nearPlane:
//     Положительное расстояние от Camera
//     до ближней clipping plane.
//
// farPlane:
//     Положительное расстояние от Camera
//     до дальней clipping plane.
//
// Требования:
//
//     verticalFovRadians finite
//
//     0 < verticalFovRadians < pi
//
//     aspectRatio finite
//
//     aspectRatio > 0
//
//     nearPlane finite
//
//     nearPlane > 0
//
//     farPlane finite
//
//     farPlane > nearPlane
//
// При некорректных параметрах функция бросает:
//
//     std::invalid_argument
//
// ------------------------------------------------------------
//
// Соглашения:
//
//     right-handed coordinate system
//
//     view-space camera direction = -Z
//
// Vulkan NDC:
//
//     X: [-1, +1]
//     Y: [-1, +1]
//     Z: [ 0, +1]
//
// После perspective divide:
//
//     near plane -> z = 0
//     far plane  -> z = 1
//
// Vulkan clip volume:
//
//     -w <= x <= w
//     -w <= y <= w
//      0 <= z <= w
//
// ------------------------------------------------------------
//
// ВАЖНО:
//
// Y внутри этой матрицы НЕ переворачивать.
//
// Renderer использует Vulkan viewport с:
//
//     viewport.height < 0
//
// чтобы согласовать экранную систему координат
// с нашей математикой.
//
[[nodiscard]]
Mat4 makePerspectiveMatrix(
    float verticalFovRadians,
    float aspectRatio,
    float nearPlane,
    float farPlane
);

} // namespace tri3d