/**
 * @file Grid.hpp
 * @ingroup gui_gameplay
 * @brief Grid components and the grid module with coordinate helpers.
 */
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

/**
 * @brief Registers grid systems and helpers.
 * @ingroup gui_gameplay
 */
struct Grid {
    /// Imports the grid module.
    Grid(flecs::world &world);

    /**
     * @brief Maps grid coordinates to a world-space position at ground level.
     * @param x Grid column index.
     * @param y Grid row index (mapped onto the world Z axis).
     * @return World-space position with y = 0, spacing each cell by its size
     *         plus padding.
     */
    static Position3 position(int x, int y);
    /**
     * @brief Returns the world-space position just above a grid tile's surface.
     * @param x Grid column index.
     * @param y Grid row index (mapped onto the world Z axis).
     * @return Same horizontal position as position() but raised to y = 0.45,
     *         suitable for placing entities on top of the tile.
     */
    static Position3 topPosition(int x, int y);
    /**
     * @brief Spawns the grid container and all of its tile entities.
     * @param world ECS world that receives the spawned entities.
     * @param width Number of tile columns.
     * @param height Number of tile rows.
     */
    static void spawn(const flecs::world &world, int width, int height);
};
