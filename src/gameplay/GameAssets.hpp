#pragma once
#include <array>
#include <raylib.h>
#include <vector>

namespace flecs {
struct world;
}

/// Stores loaded gameplay assets.
struct GameAssets {
    std::vector<Texture2D> skins;
    std::array<Model, 7> resourceModels = {};
    Model eggModel = {};

    /// Loads shared gameplay assets.
    static void load(flecs::world &world);
};
