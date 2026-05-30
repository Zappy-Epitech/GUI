#pragma once

#include "src/core/Spatial.hpp"
namespace flecs {
struct world;
}

struct Movement {
    Movement(flecs::world &world);
};

struct Direction {
    float x;
    float y;

    Direction() : x(0), y(0) {
    }

    Direction(Position3 pos) : x(pos.x), y(pos.z) {
    }
};
