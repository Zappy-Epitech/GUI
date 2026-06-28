/**
 * @file Spatial.hpp
 * @ingroup gui_core
 * @brief Spatial component definitions (positions, sizes, rotation, scale, orientation) and module.
 */
#pragma once
#include <raylib.h>

/// The window width in pixels.
static constexpr float WINDOW_WIDTH = 1920.0f;
/// The window height in pixels.
static constexpr float WINDOW_HEIGHT = 1080.0f;

namespace flecs {
struct world;
}

/// Stores a 2D position.
struct Position2 {
    float x, y;

    /// Returns the screen center.
    static Position2 center();
    /// Returns the center for a given size.
    static Position2 center(float width, float height);
    /// Returns the bottom center point.
    static Position2 bottom_center();
    /// Returns a position with both values equal.
    static Position2 splat(float value);

    /// Returns a copy with a new x value.
    Position2 with_x(float x);
    /// Returns a copy with a new y value.
    Position2 with_y(float y);
    /// Returns a copy shifted left.
    Position2 sub_x(float x);
    /// Returns a copy shifted up.
    Position2 sub_y(float y);
    /// Returns a copy shifted right.
    Position2 add_x(float x);
    /// Returns a copy shifted down.
    Position2 add_y(float y);
    /// Returns a copy with divided x.
    Position2 div_x(float x);
    /// Returns a copy with divided y.
    Position2 div_y(float y);

    /// Builds a rectangle from this position.
    Rectangle rect(float width, float height);
};

/// Stores a 3D position.
struct Position3 {
    float x, y, z;

    /// Returns the origin.
    static Position3 zero();
    /// Creates a position from coordinates.
    static Position3 from_xyz(float x, float y, float z);
    /// Returns a copy with a new x value.
    Position3 with_x(float x) const;
    /// Returns a copy with a new y value.
    Position3 with_y(float y) const;
    /// Returns a copy with a new z value.
    Position3 with_z(float z) const;
    /// Returns a copy shifted on x.
    Position3 sub_x(float x) const;
    /// Returns a copy shifted down.
    Position3 sub_y(float y) const;
    /// Returns a copy shifted on z.
    Position3 sub_z(float z) const;
    /// Returns a copy advanced on x.
    Position3 add_x(float x) const;
    /// Returns a copy advanced on y.
    Position3 add_y(float y) const;
    /// Returns a copy advanced on z.
    Position3 add_z(float z) const;
};

using Size3 = Position3;

/// Stores Euler rotation values.
struct Rotation3 {
    float x, y, z;

    /// Returns a zero rotation.
    static Rotation3 zero();
    /// Creates a rotation from angles.
    static Rotation3 from_xyz(float x, float y, float z);
};

/// Represents a grid orientation.
enum struct Orientation : int {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4,
};

/// Stores a 2D size.
struct Size2 {
    float width, height;
};

/// Stores a uniform scale.
struct Scale {
    float value;
};

/**
 * @brief ECS module registering the spatial components.
 * @details Registers Position2, Position3, Rotation3, Size2, Scale and the
 * Orientation enum (with reflected members) so they can be used and serialized
 * by other modules.
 * @ingroup gui_core
 */
struct Spatial {
    /// Imports the spatial module.
    Spatial(flecs::world &world);
};
