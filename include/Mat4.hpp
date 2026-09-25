#pragma once

#include "Vec3.hpp"

namespace tri3d {


// ============================================================
// Vec4
// ============================================================

// Четырёхмерный вектор.
//
// Нужен прежде всего для homogeneous coordinates,
// используемых 3D rendering pipeline.
//
// Точка из Vec3 представляется как:
//
//     { x, y, z, 1 }
//
// Направление представляется как:
//
//     { x, y, z, 0 }
//
// После perspective projection значение w обязательно
// сохраняется до выполнения perspective divide.
struct Vec4
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};


// Арифметика Vec4 нужна в том числе Renderer для будущего
// clipping/interpolation в homogeneous coordinates.

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
// Соглашения движка:
//
// 1. Хранение:
//
//        m[row][column]
//
// 2. Векторы считаются COLUMN vectors.
//
// 3. Вектор преобразуется так:
//
//        result = matrix * vector
//
// 4. Если используется несколько преобразований:
//
//        result = projection * view * world
//
//    то самая правая матрица применяется первой.
//
// 5. Система координат движка правосторонняя.
//
// 6. В VIEW SPACE камера смотрит вдоль -Z.
//
// 7. Canonical NDC:
//
//        X: [-1, +1]
//        Y: [-1, +1]
//        Z: [-1, +1]
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
// Basic operations
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
// и:
//
//     result * vector
//
// сначала к vector применяется B,
// затем A.
//
// Например:
//
//     projection * view
//
// означает:
//
//     world -> view -> clip
//
[[nodiscard]]
Mat4 operator*(
    const Mat4& left,
    const Mat4& right
) noexcept;


// Умножает матрицу 4x4 на Vec4.
//
// Формально:
//
//     result[row] =
//         sum(matrix[row][column] * vector[column])
//
[[nodiscard]]
Vec4 operator*(
    const Mat4& matrix,
    const Vec4& vector
) noexcept;


// ============================================================
// Homogeneous coordinates
// ============================================================

// Превращает 3D-точку в homogeneous Vec4.
//
// Возвращает:
//
//     { x, y, z, 1 }
//
// Translation matrix должна воздействовать на такую величину.
[[nodiscard]]
Vec4 pointToHomogeneous(
    const Vec3& point
) noexcept;


// Превращает 3D-направление в homogeneous Vec4.
//
// Возвращает:
//
//     { x, y, z, 0 }
//
// Translation matrix НЕ должна воздействовать на direction.
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
// Возвращённый Vec3 находится в NDC.
//
// PRECONDITION:
//
//     vector.w != 0
//
// Функция намеренно noexcept и не делает runtime-проверку,
// потому что потенциально вызывается для огромного количества
// вершин.
//
// Renderer обязан убедиться, что вершина корректна для
// perspective divide.
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
// Параметры right/up/forward считаются уже корректным
// ортонормированным базисом.
//
// Camera отвечает за выполнение этого инварианта.
//
// ------------------------------------------------------------
//
// ВАЖНО:
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
// При этом в VIEW SPACE соглашение движка:
//
//     камера смотрит вдоль -Z.
//
// ------------------------------------------------------------
//
// После преобразования:
//
//     view * pointToHomogeneous(position)
//
// должно получиться:
//
//     { 0, 0, 0, 1 }
//
// Точка перед камерой:
//
//     position + forward * distance
//
// должна получить:
//
//     viewSpace.z < 0
//
// при:
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

// Создаёт perspective projection matrix.
//
// verticalFovRadians:
//     Вертикальный угол обзора в РАДИАНАХ.
//
// aspectRatio:
//     width / height viewport.
//
// nearPlane:
//     Положительное расстояние от камеры
//     до ближней clipping plane.
//
// farPlane:
//     Положительное расстояние от камеры
//     до дальней clipping plane.
//
// Требования:
//
//     0 < verticalFovRadians < pi
//
//     aspectRatio > 0
//
//     nearPlane > 0
//
//     farPlane > nearPlane
//
// Все значения должны быть finite.
//
// При некорректном аргументе функция должна бросать:
//
//     std::invalid_argument
//
// ------------------------------------------------------------
//
// Используем:
//
//     right-handed coordinates
//
// view-space camera direction:
//
//     -Z
//
// canonical NDC:
//
//     X: [-1, +1]
//     Y: [-1, +1]
//     Z: [-1, +1]
//
// near plane после perspective divide:
//
//     z = -1
//
// far plane:
//
//     z = +1
//
// Следовательно canonical clip volume:
//
//     -w <= x <= w
//     -w <= y <= w
//     -w <= z <= w
//
// Renderer может использовать эти условия для clipping.
//
[[nodiscard]]
Mat4 makePerspectiveMatrix(
    float verticalFovRadians,
    float aspectRatio,
    float nearPlane,
    float farPlane
);

} // namespace tri3d