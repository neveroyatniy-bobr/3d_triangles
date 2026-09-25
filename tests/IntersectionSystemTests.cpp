#include "IntersectionSystem.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "UniformGrid.hpp"
#include "Vec3.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace tri3d {
namespace {

struct RandomSceneConfig
{
    const char* name;

    std::size_t triangleCount;

    Vec3 worldMin;
    Vec3 worldMax;

    float triangleExtent;
    float cellSize;

    std::size_t iterations;
};

void expect(
    bool condition,
    const std::string& message
)
{
    if (!condition)
        throw std::runtime_error(message);
}

IntersectionFlags findIntersectionsBruteForce(
    const Scene& scene
)
{
    IntersectionFlags flags(
        scene.triangles.size(),
        0
    );

    for (std::size_t i = 0; i < scene.triangles.size(); ++i)
    {
        for (std::size_t j = i + 1; j < scene.triangles.size(); ++j)
        {
            if (intersects(
                scene.triangles[i],
                scene.triangles[j]
            ))
            {
                flags[i] = 1;
                flags[j] = 1;
            }
        }
    }

    return flags;
}

float randomFloat(
    std::mt19937& generator,
    float min,
    float max
)
{
    std::uniform_real_distribution<float> distribution(
        min,
        max
    );

    return distribution(generator);
}

Vec3 randomVector(
    std::mt19937& generator,
    float extent
)
{
    return {
        randomFloat(generator, -extent, extent),
        randomFloat(generator, -extent, extent),
        randomFloat(generator, -extent, extent)
    };
}

Vec3 randomCenter(
    std::mt19937& generator,
    const Vec3& worldMin,
    const Vec3& worldMax,
    float margin
)
{
    return {
        randomFloat(
            generator,
            worldMin.x + margin,
            worldMax.x - margin
        ),
        randomFloat(
            generator,
            worldMin.y + margin,
            worldMax.y - margin
        ),
        randomFloat(
            generator,
            worldMin.z + margin,
            worldMax.z - margin
        )
    };
}

Triangle randomTriangle(
    std::mt19937& generator,
    const Vec3& worldMin,
    const Vec3& worldMax,
    float extent
)
{
    const Vec3 center =
        randomCenter(
            generator,
            worldMin,
            worldMax,
            extent
        );

    while (true)
    {
        const Vec3 firstOffset =
            randomVector(generator, extent);

        const Vec3 secondOffset =
            randomVector(generator, extent);

        const Vec3 normal =
            cross(firstOffset, secondOffset);

        const float normalSquared =
            dot(normal, normal);

        // Не даём генератору создавать почти вырожденные
        // Triangle. Сам проект пока предполагает,
        // что входные Triangle невырожденные.
        const float minimumNormalSquared =
            extent * extent *
            extent * extent *
            0.0001f;

        if (normalSquared <= minimumNormalSquared)
            continue;

        return {
            center,
            center + firstOffset,
            center + secondOffset
        };
    }
}

Scene generateRandomScene(
    const RandomSceneConfig& config,
    std::uint32_t seed
)
{
    std::mt19937 generator(seed);

    Scene scene;

    // Это именно границы мира.
    //
    // Не вычисляем их из сгенерированных Triangle:
    // UniformGrid должен получить заданные min/max
    // пространства, в котором находится Scene.
    scene.min = config.worldMin;
    scene.max = config.worldMax;

    scene.triangles.reserve(
        config.triangleCount
    );

    for (std::size_t i = 0;
         i < config.triangleCount;
         ++i)
    {
        scene.triangles.push_back(
            randomTriangle(
                generator,
                config.worldMin,
                config.worldMax,
                config.triangleExtent
            )
        );
    }

    return scene;
}

void compareResults(
    const IntersectionFlags& expected,
    const IntersectionFlags& actual,
    const RandomSceneConfig& config,
    std::uint32_t seed
)
{
    expect(
        expected.size() == actual.size(),
        "IntersectionFlags size mismatch"
    );

    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        if (expected[i] == actual[i])
            continue;

        throw std::runtime_error(
            std::string("Random test failed\n") +
            "scenario: " + config.name + "\n" +
            "seed: " + std::to_string(seed) + "\n" +
            "triangle index: " + std::to_string(i) + "\n" +
            "expected: " + std::to_string(expected[i]) + "\n" +
            "actual: " + std::to_string(actual[i])
        );
    }
}

void runRandomScenario(
    const RandomSceneConfig& config,
    std::uint32_t scenarioSeed
)
{
    for (std::size_t iteration = 0;
         iteration < config.iterations;
         ++iteration)
    {
        const std::uint32_t seed =
            scenarioSeed +
            static_cast<std::uint32_t>(iteration);

        const Scene scene =
            generateRandomScene(
                config,
                seed
            );

        const IntersectionFlags expected =
            findIntersectionsBruteForce(scene);

        const UniformGrid grid(
            scene,
            config.cellSize
        );

        const IntersectionFlags actual =
            findIntersections(
                scene,
                grid
            );

        compareResults(
            expected,
            actual,
            config,
            seed
        );
    }

    std::cout
        << "[PASS] "
        << config.name
        << " ("
        << config.iterations
        << " random scenes)"
        << '\n';
}

void testRandomScenes()
{
    const std::vector<RandomSceneConfig> configs = {

        // Очень разреженная большая сцена.
        {
            "very sparse",
            100,
            { -1000.0f, -1000.0f, -1000.0f },
            {  1000.0f,  1000.0f,  1000.0f },
            5.0f,
            25.0f,
            100
        },

        // Разреженная.
        {
            "sparse",
            120,
            { -250.0f, -200.0f, -300.0f },
            {  250.0f,  200.0f,  300.0f },
            10.0f,
            20.0f,
            100
        },

        // Средняя заполненность.
        {
            "medium",
            150,
            { -75.0f, -75.0f, -75.0f },
            {  75.0f,  75.0f,  75.0f },
            12.0f,
            10.0f,
            100
        },

        // Плотная сцена.
        {
            "dense",
            200,
            { -30.0f, -30.0f, -30.0f },
            {  30.0f,  30.0f,  30.0f },
            10.0f,
            6.0f,
            100
        },

        // Очень плотная сцена.
        {
            "very dense",
            250,
            { -15.0f, -15.0f, -15.0f },
            {  15.0f,  15.0f,  15.0f },
            7.0f,
            3.0f,
            100
        },

        // Весь мир находится в положительных координатах.
        // Ловит ошибки, где origin или bounds случайно
        // предполагаются равными нулю.
        {
            "positive world",
            150,
            { 100.0f, 200.0f, 300.0f },
            { 300.0f, 400.0f, 500.0f },
            15.0f,
            12.0f,
            100
        },

        // То же самое для полностью отрицательного мира.
        {
            "negative world",
            150,
            { -500.0f, -400.0f, -300.0f },
            { -100.0f, -80.0f, -50.0f },
            15.0f,
            12.0f,
            100
        },

        // Неравномерные размеры мира по осям.
        {
            "anisotropic world",
            180,
            { -300.0f, -40.0f, -100.0f },
            {  300.0f,  40.0f,  100.0f },
            10.0f,
            8.0f,
            100
        },

        // Cell заметно меньше типичного Triangle.
        // Один Triangle будет попадать во множество cells.
        {
            "small cells",
            150,
            { -50.0f, -50.0f, -50.0f },
            {  50.0f,  50.0f,  50.0f },
            8.0f,
            4.0f,
            100
        },

        // Cell заметно больше Triangle.
        // В одной Cell оказывается много кандидатов.
        {
            "large cells",
            150,
            { -50.0f, -50.0f, -50.0f },
            {  50.0f,  50.0f,  50.0f },
            8.0f,
            30.0f,
            100
        }
    };

    std::uint32_t scenarioSeed = 0xC0FFEEu;

    for (const RandomSceneConfig& config : configs)
    {
        runRandomScenario(
            config,
            scenarioSeed
        );

        scenarioSeed += 10000u;
    }
}

} // namespace
} // namespace tri3d


int main()
{
    try
    {
        tri3d::testRandomScenes();

        std::cout
            << "All IntersectionSystem tests passed.\n";

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << exception.what()
            << '\n';

        return 1;
    }
}