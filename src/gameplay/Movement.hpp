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
    float speed;

    /// Creates a zero direction.
    Direction() : x(0), y(0), speed(1.0f) {
    }

    /// Creates a direction from a 3D position.
    Direction(Position3 pos) : x(pos.x), y(pos.z), speed(1.0f) {
    }

    /// Creates a direction from a 3D position and movement speed.
    Direction(Position3 pos, float speed) : x(pos.x), y(pos.z), speed(speed) {
    }
};

/// Returns the player interpolation speed for a movement distance and server frequency.
float movementSpeedForFrequency(float distance, int frequency);
