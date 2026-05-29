#pragma once

namespace flecs {
struct world;
}

struct Movement {
    Movement(flecs::world &world);
};

struct Direction {
    float x;
    float y;
};
