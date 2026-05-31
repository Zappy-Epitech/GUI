#pragma once
#include "src/core/Spatial.hpp"

namespace flecs {
struct world;
}

/// Marks a grid tile entity.
struct GridCell {};

/// Marks the grid root entity.
struct GridContainer {};

/// Stores grid coordinates.
struct GridPosition {
    int x;
    int y;
};

/// Registers grid systems and helpers.
struct Grid {
    /// Imports the grid module.
    Grid(flecs::world &world);

    /// Returns a world position for grid coordinates.
    static Position3 position(int x, int y);
    /// Returns the top position of a grid tile.
    static Position3 topPosition(int x, int y);
    /// Spawns a grid with the given size.
    static void spawn(const flecs::world &world, int width, int height);
};
