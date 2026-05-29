#pragma once

namespace flecs {
struct world;
}

struct MinecraftSkin {
    float scale = 1.0f;
};

struct MinecraftRenderer {
    MinecraftRenderer(flecs::world &world);
};
