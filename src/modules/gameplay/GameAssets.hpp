#pragma once
#include <array>
#include <raylib.h>
#include <vector>

namespace flecs {
struct world;
}

struct GameAssets {
    std::vector<Texture2D> skins;
    std::array<Model, 7> resourceModels = {};

    static void load(flecs::world &world);
};
