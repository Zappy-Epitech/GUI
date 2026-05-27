#pragma once
#include "Spatial.hpp"

namespace flecs {
struct world;
}

struct Cube {};

struct Raylib {
    Raylib(flecs::world &world);
};
