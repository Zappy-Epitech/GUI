/**
 * @file GameAssets.hpp
 * @ingroup gui_gameplay
 * @brief Shared gameplay assets singleton (skins, resource models, icons).
 */
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

/**
 * @brief Holds loaded gameplay assets shared across the scene.
 * @ingroup gui_gameplay
 */
struct GameAssets {
    std::vector<SkinAsset> skins;
    std::array<Model, 7> resourceModels = {};
    std::array<Texture2D, 7> resourceIconTextures = {};
    Texture2D inventoryGuiTexture = {};
    Model eggModel = {};

    /// Loads shared gameplay assets.
    static void load(flecs::world &world);
};
