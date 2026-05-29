#pragma once
#include <raylib.h>
#include <vector>

namespace flecs {
struct world;
}

struct GameAssets {
    std::vector<Texture2D> skins;

    static void load(flecs::world &world);
};
