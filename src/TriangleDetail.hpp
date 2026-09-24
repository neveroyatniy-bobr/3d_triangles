#pragma once

#include "Triangle.hpp"

namespace tri3d
{

struct TrianglePlaneOrientation
{
    float a;
    float b;
    float c;
};


struct TriangleReordering
{
    Triangle triangle;
    bool reverseOtherTriangle = false;
};


struct CanonicalTriangles
{
    Triangle first;
    Triangle second;
};


struct Vec2
{
    float x;
    float y;
};


struct Triangle2D
{
    Vec2 a;
    Vec2 b;
    Vec2 c;
};


// ------------------------------------------------------------
// Plane classification
// ------------------------------------------------------------

[[nodiscard]]
TrianglePlaneOrientation orientTriangleAgainstPlane(
    const Triangle& triangle,
    const Triangle& planeTriangle
) noexcept;


[[nodiscard]]
bool liesStrictlyOnOneSide(
    const TrianglePlaneOrientation& orientation
) noexcept;


[[nodiscard]]
bool isCoplanar(
    const TrianglePlaneOrientation& orientation
) noexcept;


// ------------------------------------------------------------
// Non-coplanar intersection
// ------------------------------------------------------------

[[nodiscard]]
bool nonCoplanarTrianglesIntersect(
    const Triangle& first,
    const Triangle& second,
    const TrianglePlaneOrientation& firstOrientation,
    const TrianglePlaneOrientation& secondOrientation
) noexcept;


[[nodiscard]]
CanonicalTriangles canonicalizeTriangles(
    const Triangle& first,
    const Triangle& second,
    const TrianglePlaneOrientation& firstOrientation,
    const TrianglePlaneOrientation& secondOrientation
) noexcept;


[[nodiscard]]
TriangleReordering reorderTriangleVertices(
    const Triangle& triangle,
    const TrianglePlaneOrientation& orientation
) noexcept;


void reverseWinding(
    Triangle& triangle
) noexcept;


[[nodiscard]]
bool canonicalTrianglesOverlap(
    const Triangle& first,
    const Triangle& second
) noexcept;


// ------------------------------------------------------------
// Coplanar intersection
// ------------------------------------------------------------

[[nodiscard]]
bool coplanarTrianglesIntersect(
    const Triangle& first,
    const Triangle& second
) noexcept;


[[nodiscard]]
bool triangleEdgesIntersect(
    const Triangle2D& first,
    const Triangle2D& second
) noexcept;


[[nodiscard]]
bool segmentsIntersect(
    const Vec2& firstA,
    const Vec2& firstB,
    const Vec2& secondA,
    const Vec2& secondB
) noexcept;


[[nodiscard]]
bool pointOnSegment(
    const Vec2& segmentA,
    const Vec2& segmentB,
    const Vec2& point
) noexcept;


[[nodiscard]]
bool pointInsideTriangle(
    const Vec2& point,
    const Triangle2D& triangle
) noexcept;


// ------------------------------------------------------------
// Basic orientation predicates
// ------------------------------------------------------------

[[nodiscard]]
float orient3D(
    const Vec3& a,
    const Vec3& b,
    const Vec3& c,
    const Vec3& point
) noexcept;


[[nodiscard]]
float orient2D(
    const Vec2& a,
    const Vec2& b,
    const Vec2& point
) noexcept;

}