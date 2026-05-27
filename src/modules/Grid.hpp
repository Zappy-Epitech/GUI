#pragma once
#include <sys/types.h>

namespace flecs {
struct world;
};

void spawnGrid(flecs::world &world, uint width, uint height);
