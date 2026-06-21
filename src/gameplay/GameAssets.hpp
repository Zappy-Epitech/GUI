#pragma once
#include <array>
#include <raylib.h>
#include <string>
#include <vector>

namespace flecs {
struct world;
}

/// Stores loaded gameplay assets.
struct SkinAsset {
    std::string name;
    Texture2D texture;
};

struct GameAssets {
    std::vector<SkinAsset> skins;
    std::array<Model, 7> resourceModels = {};
    Model eggModel = {};

    /// Loads shared gameplay assets.
    static void load(flecs::world &world);
};
