#pragma once
#include "src/modules/Spatial.hpp"

namespace flecs {
struct world;
};

struct GridCell {};
struct GridContainer {};
struct GridPosition {
    int x;
    int y;
};

struct Grid {
    Grid(flecs::world &world);

    static Position3 position(int x, int y);
    static Position3 topPosition(int x, int y);
    static void spawn(const flecs::world &world, int width, int height);
};
