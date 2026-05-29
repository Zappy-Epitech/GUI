#pragma once

namespace flecs {
struct world;
};

struct GridCell {};
struct GridContainer {};

struct Grid {
    Grid(flecs::world &world);

    static void spawn(const flecs::world &world, int width, int height);
};
