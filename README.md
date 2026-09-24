# tri3d_engine

Учебный C++20/CMake-проект простого 3D-движка для хранения до 1 000 000 треугольников,
поиска пересечений и отображения результата.

На этом этапе зафиксированы только архитектура и публичные интерфейсы.
Алгоритмы пересечения, построение Uniform Grid и графический backend еще не реализованы.

## Структура

```text
tri3d_engine/
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   ├── Vec3.hpp
│   ├── Triangle.hpp
│   ├── Scene.hpp
│   ├── Camera.hpp
│   ├── UniformGrid.hpp
│   ├── IntersectionSystem.hpp
│   ├── Renderer.hpp
│   └── Engine.hpp
└── src/
    ├── main.cpp
    ├── Scene.cpp
    ├── Camera.cpp
    ├── UniformGrid.cpp
    ├── IntersectionSystem.cpp
    ├── Renderer.cpp
    └── Engine.cpp
```

## Основной поток данных

```text
Scene
  ↓
UniformGrid::rebuild(scene)
  ↓
UniformGrid::occupiedCells()
  ↓
IntersectionSystem::calculate(...)
  ↓
IntersectionFlags
  ↓
Renderer::render(...)
```

## Принятый способ работы Uniform Grid

Grid хранит индексы треугольников в каждой непустой ячейке.

IntersectionSystem проходит по каждой ячейке и перебирает все неупорядоченные пары
треугольников внутри нее:

```text
cell = [A, B, C, D]

A-B
A-C
A-D
B-C
B-D
C-D
```

Если AABB двух больших треугольников попали сразу в несколько одинаковых ячеек,
одна и та же пара может быть проверена повторно. На первом этапе дедупликации нет:
повторная проверка не меняет корректность флагов пересечения, а усложнение будет
добавляться только при подтвержденной необходимости.

## Зачем std::span в Scene

`Scene` владеет памятью через `std::vector<Triangle>`.

```cpp
std::span<const Triangle> triangles() const noexcept;
```

`std::span` — невладеющее представление непрерывной последовательности элементов.
Он содержит указатель на данные и количество элементов, но не копирует треугольники
и не владеет ими.

Это дает нам одновременно:

- `Scene` продолжает владеть `std::vector`;
- внешний код может эффективно пройти по всем Triangle;
- наружу не раскрывается сам объект `std::vector`;
- `const Triangle` запрещает изменение геометрии через этот интерфейс;
- нет копирования даже для миллиона треугольников.

Важно: span остается корректным только пока underlying `std::vector` не был
перевыделен/уничтожен. Поэтому его не нужно сохранять надолго между изменениями Scene.

## C++20

`std::span` появился в C++20. CMake уже требует C++20:

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Если IDE подчеркивает `std::span`, нужно открыть именно корневой `CMakeLists.txt`
как CMake-проект и дать IDE выполнить configure/reload.
