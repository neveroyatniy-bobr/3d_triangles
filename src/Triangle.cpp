#include "Triangle.hpp"
#include "TriangleDetail.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace tri3d
{

// ------------------------------------------------------------
// Triangle intersection
// ------------------------------------------------------------

bool intersects(
    const Triangle& first,
    const Triangle& second
) noexcept
{
    // Определяем положение первого Triangle
    // относительно плоскости второго.
    const TrianglePlaneOrientation firstOrientation =
        orientTriangleAgainstPlane(
            first,
            second
        );


    // Если весь первый Triangle находится строго
    // с одной стороны плоскости второго,
    // пересечения быть не может.
    if (liesStrictlyOnOneSide(firstOrientation))
    {
        return false;
    }


    // Если весь первый Triangle лежит
    // в плоскости второго, Triangle копланарны.
    if (isCoplanar(firstOrientation))
    {
        return coplanarTrianglesIntersect(
            first,
            second
        );
    }


    // Теперь определяем положение второго Triangle
    // относительно плоскости первого.
    const TrianglePlaneOrientation secondOrientation =
        orientTriangleAgainstPlane(
            second,
            first
        );


    if (liesStrictlyOnOneSide(secondOrientation))
    {
        return false;
    }


    // Оба Triangle пересекают плоскость друг друга.
    // Приводим их к каноническому расположению
    // и выполняем финальную orientation-проверку.
    return nonCoplanarTrianglesIntersect(
        first,
        second,
        firstOrientation,
        secondOrientation
    );
}


// ------------------------------------------------------------
// Plane classification
// ------------------------------------------------------------

TrianglePlaneOrientation orientTriangleAgainstPlane(
    const Triangle& triangle,
    const Triangle& planeTriangle
) noexcept
{
    const Vec3 normal =
        cross(
            planeTriangle.b - planeTriangle.a,
            planeTriangle.c - planeTriangle.a
        );


    return {
        dot(
            triangle.a - planeTriangle.a,
            normal
        ),

        dot(
            triangle.b - planeTriangle.a,
            normal
        ),

        dot(
            triangle.c - planeTriangle.a,
            normal
        )
    };
}


bool liesStrictlyOnOneSide(
    const TrianglePlaneOrientation& orientation
) noexcept
{
    const bool allPositive =
        orientation.a > 0.0f &&
        orientation.b > 0.0f &&
        orientation.c > 0.0f;


    const bool allNegative =
        orientation.a < 0.0f &&
        orientation.b < 0.0f &&
        orientation.c < 0.0f;


    return allPositive || allNegative;
}


bool isCoplanar(
    const TrianglePlaneOrientation& orientation
) noexcept
{
    return
        orientation.a == 0.0f &&
        orientation.b == 0.0f &&
        orientation.c == 0.0f;
}


// ------------------------------------------------------------
// Non-coplanar intersection
// ------------------------------------------------------------

bool nonCoplanarTrianglesIntersect(
    const Triangle& first,
    const Triangle& second,
    const TrianglePlaneOrientation& firstOrientation,
    const TrianglePlaneOrientation& secondOrientation
) noexcept
{
    const CanonicalTriangles canonical =
        canonicalizeTriangles(
            first,
            second,
            firstOrientation,
            secondOrientation
        );


    return canonicalTrianglesOverlap(
        canonical.first,
        canonical.second
    );
}


// ------------------------------------------------------------
// Canonicalization
// ------------------------------------------------------------

CanonicalTriangles canonicalizeTriangles(
    const Triangle& first,
    const Triangle& second,
    const TrianglePlaneOrientation& firstOrientation,
    const TrianglePlaneOrientation& secondOrientation
) noexcept
{
    // Сначала переставляем вершины первого Triangle
    // в необходимый циклический порядок.
    TriangleReordering firstReordering =
        reorderTriangleVertices(
            first,
            firstOrientation
        );


    Triangle preparedSecond =
        second;

    TrianglePlaneOrientation preparedSecondOrientation =
        secondOrientation;


    // Иногда для согласования ориентации пары
    // необходимо изменить направление обхода
    // второго Triangle.
    if (firstReordering.reverseOtherTriangle)
    {
        reverseWinding(
            preparedSecond
        );

        // Вершины B и C поменялись местами,
        // значит их orientation тоже меняются местами.
        std::swap(
            preparedSecondOrientation.b,
            preparedSecondOrientation.c
        );
    }


    // Теперь приводим в канонический порядок
    // второй Triangle.
    const TriangleReordering secondReordering =
        reorderTriangleVertices(
            preparedSecond,
            preparedSecondOrientation
        );


    Triangle canonicalFirst =
        firstReordering.triangle;


    // Канонизация второго Triangle может потребовать
    // изменения направления обхода первого.
    if (secondReordering.reverseOtherTriangle)
    {
        reverseWinding(
            canonicalFirst
        );
    }


    return {
        canonicalFirst,
        secondReordering.triangle
    };
}


TriangleReordering reorderTriangleVertices(
    const Triangle& triangle,
    const TrianglePlaneOrientation& orientation
) noexcept
{
    const std::array<const Vec3*, 3> vertices{
        &triangle.a,
        &triangle.b,
        &triangle.c
    };


    const std::array<float, 3> sides{
        orientation.a,
        orientation.b,
        orientation.c
    };


    // У Triangle существует только три циклических
    // расположения вершин.
    //
    // Перебираем их и ищем такое, в котором
    // первая вершина становится опорной
    // для orientation-алгоритма.
    for (std::size_t i = 0; i < 3; ++i)
    {
        const std::size_t next =
            (i + 1) % 3;

        const std::size_t last =
            (i + 2) % 3;


        const float firstSide =
            sides[i];

        const float secondSide =
            sides[next];

        const float thirdSide =
            sides[last];


        const bool positivePivot =
            firstSide > 0.0f &&
            secondSide <= 0.0f &&
            thirdSide <= 0.0f;


        const bool negativePivot =
            firstSide < 0.0f &&
            secondSide >= 0.0f &&
            thirdSide >= 0.0f;


        const bool planePivot =
            firstSide == 0.0f &&
            (
                (
                    secondSide > 0.0f &&
                    thirdSide > 0.0f
                )
                ||
                (
                    secondSide < 0.0f &&
                    thirdSide < 0.0f
                )
            );


        if (!positivePivot &&
            !negativePivot &&
            !planePivot)
        {
            continue;
        }


        const Triangle reordered{
            *vertices[i],
            *vertices[next],
            *vertices[last]
        };


        const bool reverseOtherTriangle =
            firstSide < 0.0f
            ||
            (
                firstSide == 0.0f &&
                secondSide > 0.0f
            );


        return {
            reordered,
            reverseOtherTriangle
        };
    }


    // Для невырожденного некопланарного Triangle
    // сюда попасть не должны.
    return {
        triangle,
        false
    };
}


void reverseWinding(
    Triangle& triangle
) noexcept
{
    std::swap(
        triangle.b,
        triangle.c
    );
}


// ------------------------------------------------------------
// Final canonical test
// ------------------------------------------------------------

bool canonicalTrianglesOverlap(
    const Triangle& first,
    const Triangle& second
) noexcept
{
    // После канонизации нам не нужно
    // явно вычислять линию пересечения плоскостей
    // и интервалы Triangle на этой линии.
    //
    // Их взаимный порядок определяется
    // двумя orientation-предикатами.

    if (orient3D(
            first.b,
            second.a,
            first.a,
            second.b
        ) > 0.0f)
    {
        return false;
    }


    if (orient3D(
            first.a,
            second.a,
            first.c,
            second.c
        ) > 0.0f)
    {
        return false;
    }


    return true;
}


// ------------------------------------------------------------
// Coplanar intersection
// ------------------------------------------------------------

bool coplanarTrianglesIntersect(
    const Triangle& first,
    const Triangle& second
) noexcept
{
    const Vec3 normal =
        cross(
            first.b - first.a,
            first.c - first.a
        );


    const float absX =
        std::abs(normal.x);

    const float absY =
        std::abs(normal.y);

    const float absZ =
        std::abs(normal.z);


    // Выбрасываем координату, соответствующую
    // наибольшей компоненте нормали.
    //
    // Так Triangle сохраняет максимальную
    // площадь после проекции в 2D.
    int droppedAxis;

    if (absX >= absY &&
        absX >= absZ)
    {
        droppedAxis = 0;
    }
    else if (absY >= absZ)
    {
        droppedAxis = 1;
    }
    else
    {
        droppedAxis = 2;
    }


    const auto projectTo2D =
        [droppedAxis](const Vec3& point) noexcept -> Vec2
    {
        if (droppedAxis == 0)
        {
            // YZ
            return {
                point.y,
                point.z
            };
        }


        if (droppedAxis == 1)
        {
            // XZ
            return {
                point.x,
                point.z
            };
        }


        // XY
        return {
            point.x,
            point.y
        };
    };


    const Triangle2D first2D{
        projectTo2D(first.a),
        projectTo2D(first.b),
        projectTo2D(first.c)
    };


    const Triangle2D second2D{
        projectTo2D(second.a),
        projectTo2D(second.b),
        projectTo2D(second.c)
    };


    // Сначала проверяем пересечение рёбер.
    if (triangleEdgesIntersect(
            first2D,
            second2D
        ))
    {
        return true;
    }


    // Если рёбра не пересеклись,
    // один Triangle может полностью
    // находиться внутри другого.
    if (pointInsideTriangle(
            first2D.a,
            second2D
        ))
    {
        return true;
    }


    if (pointInsideTriangle(
            second2D.a,
            first2D
        ))
    {
        return true;
    }


    return false;
}


bool triangleEdgesIntersect(
    const Triangle2D& first,
    const Triangle2D& second
) noexcept
{
    const std::array<Vec2, 3> firstVertices{
        first.a,
        first.b,
        first.c
    };


    const std::array<Vec2, 3> secondVertices{
        second.a,
        second.b,
        second.c
    };


    for (std::size_t firstEdge = 0;
         firstEdge < 3;
         ++firstEdge)
    {
        const std::size_t firstNext =
            (firstEdge + 1) % 3;


        for (std::size_t secondEdge = 0;
             secondEdge < 3;
             ++secondEdge)
        {
            const std::size_t secondNext =
                (secondEdge + 1) % 3;


            if (segmentsIntersect(
                    firstVertices[firstEdge],
                    firstVertices[firstNext],
                    secondVertices[secondEdge],
                    secondVertices[secondNext]
                ))
            {
                return true;
            }
        }
    }


    return false;
}


bool segmentsIntersect(
    const Vec2& firstA,
    const Vec2& firstB,
    const Vec2& secondA,
    const Vec2& secondB
) noexcept
{
    const float firstToSecondA =
        orient2D(
            firstA,
            firstB,
            secondA
        );


    const float firstToSecondB =
        orient2D(
            firstA,
            firstB,
            secondB
        );


    const float secondToFirstA =
        orient2D(
            secondA,
            secondB,
            firstA
        );


    const float secondToFirstB =
        orient2D(
            secondA,
            secondB,
            firstB
        );


    const bool secondPointsOnOppositeSides =
        (
            firstToSecondA > 0.0f &&
            firstToSecondB < 0.0f
        )
        ||
        (
            firstToSecondA < 0.0f &&
            firstToSecondB > 0.0f
        );


    const bool firstPointsOnOppositeSides =
        (
            secondToFirstA > 0.0f &&
            secondToFirstB < 0.0f
        )
        ||
        (
            secondToFirstA < 0.0f &&
            secondToFirstB > 0.0f
        );


    if (secondPointsOnOppositeSides &&
        firstPointsOnOppositeSides)
    {
        return true;
    }


    // Касание также считается пересечением.

    if (firstToSecondA == 0.0f &&
        pointOnSegment(
            firstA,
            firstB,
            secondA
        ))
    {
        return true;
    }


    if (firstToSecondB == 0.0f &&
        pointOnSegment(
            firstA,
            firstB,
            secondB
        ))
    {
        return true;
    }


    if (secondToFirstA == 0.0f &&
        pointOnSegment(
            secondA,
            secondB,
            firstA
        ))
    {
        return true;
    }


    if (secondToFirstB == 0.0f &&
        pointOnSegment(
            secondA,
            secondB,
            firstB
        ))
    {
        return true;
    }


    return false;
}


bool pointOnSegment(
    const Vec2& segmentA,
    const Vec2& segmentB,
    const Vec2& point
) noexcept
{
    return
        point.x >= std::min(
            segmentA.x,
            segmentB.x
        )
        &&
        point.x <= std::max(
            segmentA.x,
            segmentB.x
        )
        &&
        point.y >= std::min(
            segmentA.y,
            segmentB.y
        )
        &&
        point.y <= std::max(
            segmentA.y,
            segmentB.y
        );
}


bool pointInsideTriangle(
    const Vec2& point,
    const Triangle2D& triangle
) noexcept
{
    const float firstSide =
        orient2D(
            triangle.a,
            triangle.b,
            point
        );


    const float secondSide =
        orient2D(
            triangle.b,
            triangle.c,
            point
        );


    const float thirdSide =
        orient2D(
            triangle.c,
            triangle.a,
            point
        );


    const bool hasPositive =
        firstSide > 0.0f ||
        secondSide > 0.0f ||
        thirdSide > 0.0f;


    const bool hasNegative =
        firstSide < 0.0f ||
        secondSide < 0.0f ||
        thirdSide < 0.0f;


    return !(hasPositive && hasNegative);
}


// ------------------------------------------------------------
// Basic orientation predicates
// ------------------------------------------------------------

float orient3D(
    const Vec3& a,
    const Vec3& b,
    const Vec3& c,
    const Vec3& point
) noexcept
{
    return dot(
        point - a,
        cross(
            b - a,
            c - a
        )
    );
}


float orient2D(
    const Vec2& a,
    const Vec2& b,
    const Vec2& point
) noexcept
{
    return
        (b.x - a.x) *
        (point.y - a.y)
        -
        (b.y - a.y) *
        (point.x - a.x);
}

}