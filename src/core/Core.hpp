#pragma once

namespace flecs {
struct world;
}

struct Lifetime {
    float remaining = 0.0f;
};

struct Core {
    Core(flecs::world &world);
};
