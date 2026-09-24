#include "Vec3.hpp"

#include <cmath>
#include <stdexcept>

namespace tri3d
{

Vec3 operator+(const Vec3& a, const Vec3& b) noexcept
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}


Vec3 operator-(const Vec3& a, const Vec3& b) noexcept
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}


Vec3 operator*(const Vec3& vector, float scalar) noexcept
{
    return {
        vector.x * scalar,
        vector.y * scalar,
        vector.z * scalar
    };
}


Vec3 operator*(float scalar, const Vec3& vector) noexcept
{
    return vector * scalar;
}


Vec3 operator/(const Vec3& vector, float scalar)
{
    if (scalar == 0.0f)
    {
        throw std::domain_error("Vec3 division by zero");
    }

    return {
        vector.x / scalar,
        vector.y / scalar,
        vector.z / scalar
    };
}


float dot(const Vec3& a, const Vec3& b) noexcept
{
    return
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
}


Vec3 cross(const Vec3& a, const Vec3& b) noexcept
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}


float length(const Vec3& vector) noexcept
{
    return std::hypot(
        vector.x,
        vector.y,
        vector.z
    );
}


Vec3 normalize(const Vec3& vector)
{
    const float vectorLength = length(vector);

    if (vectorLength == 0.0f)
    {
        throw std::domain_error("Cannot normalize zero vector");
    }

    return vector / vectorLength;
}

}