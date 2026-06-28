/**
 * @file Movement.hpp
 * @ingroup gui_gameplay
 * @brief Movement interpolation module, Direction component and speed helper.
 */
#pragma once

#include "src/core/Spatial.hpp"
namespace flecs {
struct world;
}

/**
 * @brief Registers movement interpolation systems.
 * @ingroup gui_gameplay
 */
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

/**
 * @brief Returns the player interpolation speed for a movement distance and
 *        server frequency.
 * @param distance World-space distance to be travelled by the move.
 * @param frequency Server tick frequency (ticks per second); clamped to
 *        [1, 1000] before use.
 * @return Speed in world units per second, computed so the move completes over
 *         a duration clamped to [0.08s, 1.2s].
 */
float movementSpeedForFrequency(float distance, int frequency);
