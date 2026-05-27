#pragma once

namespace flecs {
struct world;
}

struct Position2 {
    float x, y;
};
struct Size2 {
    float width, height;
};

struct Spatial {
    Spatial(flecs::world &world);
};
