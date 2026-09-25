#include "Triangle.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

using tri3d::Triangle;
using tri3d::Vec3;
using tri3d::intersects;


namespace
{

int failedTests = 0;


// ------------------------------------------------------------
// Test utilities
// ------------------------------------------------------------

void expect(
    bool actual,
    bool expected,
    const std::string& testName
)
{
    if (actual == expected)
        return;

    ++failedTests;

    std::cerr
        << "[FAILED] "
        << testName
        << "\n"
        << "Expected: "
        << expected
        << "\n"
        << "Actual:   "
        << actual
        << "\n\n";
}


Triangle permuteTriangle(
    const Triangle& triangle,
    const std::array<std::size_t, 3>& permutation
)
{
    const std::array<Vec3, 3> vertices{
        triangle.a,
        triangle.b,
        triangle.c
    };

    return {
        vertices[permutation[0]],
        vertices[permutation[1]],
        vertices[permutation[2]]
    };
}


// Проверяет:
//
// 1. intersects(A, B)
// 2. intersects(B, A)
// 3. все 6 перестановок вершин A
// 4. все 6 перестановок вершин B
//
// Всего для одного геометрического случая:
// 6 * 6 * 2 = 72 проверки.
void expectIntersectionForAllVertexOrders(
    const Triangle& first,
    const Triangle& second,
    bool expected,
    const std::string& testName
)
{
    constexpr std::array<
        std::array<std::size_t, 3>,
        6
    > permutations{{
        {0, 1, 2},
        {0, 2, 1},
        {1, 0, 2},
        {1, 2, 0},
        {2, 0, 1},
        {2, 1, 0}
    }};


    for (const auto& firstPermutation : permutations)
    {
        const Triangle permutedFirst =
            permuteTriangle(
                first,
                firstPermutation
            );


        for (const auto& secondPermutation : permutations)
        {
            const Triangle permutedSecond =
                permuteTriangle(
                    second,
                    secondPermutation
                );


            expect(
                intersects(
                    permutedFirst,
                    permutedSecond
                ),
                expected,
                testName + " [A -> B]"
            );


            // Пересечение обязано быть симметричным.
            expect(
                intersects(
                    permutedSecond,
                    permutedFirst
                ),
                expected,
                testName + " [B -> A]"
            );
        }
    }
}


// ------------------------------------------------------------
// Coplanar tests
// ------------------------------------------------------------

void testCoplanarSeparated()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };

    const Triangle second{
        {6.0f, 6.0f, 0.0f},
        {8.0f, 6.0f, 0.0f},
        {6.0f, 8.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        false,
        "Coplanar separated"
    );
}


void testCoplanarOverlap()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };

    const Triangle second{
        {1.0f, 0.0f, 0.0f},
        {5.0f, 0.0f, 0.0f},
        {1.0f, 4.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Coplanar overlap"
    );
}


void testCoplanarContainment()
{
    const Triangle outer{
        {0.0f, 0.0f, 0.0f},
        {8.0f, 0.0f, 0.0f},
        {0.0f, 8.0f, 0.0f}
    };

    const Triangle inner{
        {1.0f, 1.0f, 0.0f},
        {2.0f, 1.0f, 0.0f},
        {1.0f, 2.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        outer,
        inner,
        true,
        "Coplanar containment"
    );
}


void testCoplanarTouchAtVertex()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, 0.0f},
        {0.0f, 2.0f, 0.0f}
    };

    const Triangle second{
        {2.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {2.0f, 2.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Coplanar vertex touch"
    );
}


void testCoplanarSharedEdge()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {2.0f, 3.0f, 0.0f}
    };

    const Triangle second{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {2.0f, -3.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Coplanar shared edge"
    );
}


// ------------------------------------------------------------
// Non-coplanar tests
// ------------------------------------------------------------

void testNonCoplanarIntersection()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    // Вертикальный Triangle.
    //
    // Его плоскость x = 1 пересекает плоскость
    // первого Triangle по линии x = 1, z = 0.
    //
    // Отрезки Triangle на этой линии перекрываются.
    const Triangle second{
        {1.0f, 1.0f, -2.0f},
        {1.0f, 1.0f,  2.0f},
        {1.0f, 3.0f,  0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Non-coplanar intersection"
    );
}


void testNonCoplanarDisjointIntervals()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    // Обе плоскости пересекают оба Triangle,
    // поэтому простой plane-side reject не срабатывает.
    //
    // Но интервалы Triangle на линии пересечения
    // плоскостей разделены.
    const Triangle second{
        {1.0f, 5.0f, -2.0f},
        {1.0f, 5.0f,  2.0f},
        {1.0f, 7.0f,  0.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        false,
        "Non-coplanar disjoint intervals"
    );
}


void testParallelPlanes()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    const Triangle second{
        {0.0f, 0.0f, 1.0f},
        {4.0f, 0.0f, 1.0f},
        {0.0f, 4.0f, 1.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        false,
        "Parallel separated planes"
    );
}


void testNonCoplanarVertexTouch()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    // Единственная общая точка:
    //
    // (4, 0, 0)
    const Triangle second{
        {4.0f,  0.0f, 0.0f},
        {4.0f,  1.0f, 2.0f},
        {4.0f, -1.0f, 2.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Non-coplanar vertex touch"
    );
}


void testNonCoplanarSharedEdge()
{
    const Triangle first{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    // Общий отрезок:
    //
    // (0,0,0) ---- (4,0,0)
    //
    // Но плоскости Triangle различны.
    const Triangle second{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, 3.0f}
    };


    expectIntersectionForAllVertexOrders(
        first,
        second,
        true,
        "Non-coplanar shared edge"
    );
}


// ------------------------------------------------------------
// Identity
// ------------------------------------------------------------

void testIdenticalTriangles()
{
    const Triangle triangle{
        {0.0f, 0.0f, 0.0f},
        {4.0f, 0.0f, 0.0f},
        {0.0f, 4.0f, 0.0f}
    };


    expectIntersectionForAllVertexOrders(
        triangle,
        triangle,
        true,
        "Identical triangles"
    );
}

} // namespace


int main()
{
    testCoplanarSeparated();
    testCoplanarOverlap();
    testCoplanarContainment();
    testCoplanarTouchAtVertex();
    testCoplanarSharedEdge();

    testNonCoplanarIntersection();
    testNonCoplanarDisjointIntervals();
    testParallelPlanes();
    testNonCoplanarVertexTouch();
    testNonCoplanarSharedEdge();

    testIdenticalTriangles();


    if (failedTests == 0)
    {
        std::cout
            << "All Triangle intersection tests passed.\n";

        return 0;
    }


    std::cerr
        << failedTests
        << " Triangle intersection checks failed.\n";

    return 1;
}