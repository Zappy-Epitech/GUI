#pragma once
#include <sys/types.h>

namespace flecs {
struct world;
};

struct GridCell {};

struct Grid {
    Grid(flecs::world &world);

    static void spawn(flecs::world &world, uint width, uint height);
};
