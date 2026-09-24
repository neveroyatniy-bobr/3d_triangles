#pragma once

namespace tri3d
{

struct Vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};


[[nodiscard]]
Vec3 operator+(const Vec3& a, const Vec3& b) noexcept;

[[nodiscard]]
Vec3 operator-(const Vec3& a, const Vec3& b) noexcept;

[[nodiscard]]
Vec3 operator*(const Vec3& vector, float scalar) noexcept;

[[nodiscard]]
Vec3 operator*(float scalar, const Vec3& vector) noexcept;

[[nodiscard]]
Vec3 operator/(const Vec3& vector, float scalar);


[[nodiscard]]
float dot(const Vec3& a, const Vec3& b) noexcept;

[[nodiscard]]
Vec3 cross(const Vec3& a, const Vec3& b) noexcept;

[[nodiscard]]
float length(const Vec3& vector) noexcept;

[[nodiscard]]
Vec3 normalize(const Vec3& vector);

}