#pragma once

#include "src/core/Spatial.hpp"
namespace flecs {
struct world;
}

/// Registers movement systems.
struct Movement {
    /// Imports the movement module.
    Movement(flecs::world &world);
};

/// Stores a movement target direction.
struct Direction {
    float x;
    float y;

    /// Creates a zero direction.
    Direction() : x(0), y(0) {
    }

    /// Creates a direction from a 3D position.
    Direction(Position3 pos) : x(pos.x), y(pos.z) {
    }
};
