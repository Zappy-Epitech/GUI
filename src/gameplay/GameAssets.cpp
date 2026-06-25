#include "GameAssets.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include <algorithm>
#include <filesystem>
#include <vector>

/// Loads skins and resource models.
void GameAssets::load(flecs::world &world) {
    world.singleton<GameAssets>()
        .set<GameAssets>({})
        .child_of<GamePlay>();

    world.system<GameAssets>("LoadAssets")
        .term_at(0)
        .self()
        .kind(flecs::OnStart)
        .each([](GameAssets &assets) {
            std::vector<std::filesystem::path> skinPaths;
            for (const auto &file : std::filesystem::directory_iterator("./assets/skins/")) {
                if (file.is_regular_file()) {
                    skinPaths.push_back(file.path());
                }
            }

            std::sort(skinPaths.begin(), skinPaths.end());
            for (const auto &path : skinPaths) {
                Texture2D texture = LoadTexture(path.c_str());
                assets.skins.push_back(SkinAsset{ path.stem().string(), texture });
            }

            assets.resourceModels[0] = LoadModel("./assets/models/apple/scene.gltf");
            assets.resourceModels[1] = LoadModel("./assets/models/emerald/scene.gltf");
            assets.resourceModels[2] = LoadModel("./assets/models/coal/scene.gltf");
            assets.resourceModels[3] = LoadModel("./assets/models/iron/scene.gltf");
            assets.resourceModels[4] = LoadModel("./assets/models/gold/scene.gltf");
            assets.resourceModels[5] = LoadModel("./assets/models/diamond/scene.gltf");
            assets.resourceModels[6] = LoadModel("./assets/models/redstone/Redstone_Dust.gltf");
            assets.resourceIconTextures[0] = LoadTexture("./assets/minecraft/item/apple.png");
            assets.resourceIconTextures[1] = LoadTexture("./assets/minecraft/item/emerald.png");
            assets.resourceIconTextures[2] = LoadTexture("./assets/minecraft/item/coal.png");
            assets.resourceIconTextures[3] = LoadTexture("./assets/minecraft/item/iron_ingot.png");
            assets.resourceIconTextures[4] = LoadTexture("./assets/minecraft/item/gold_ingot.png");
            assets.resourceIconTextures[5] = LoadTexture("./assets/minecraft/item/diamond.png");
            assets.resourceIconTextures[6] = LoadTexture("./assets/minecraft/item/redstone.png");
            assets.inventoryGuiTexture = LoadTexture("./assets/minecraft/gui/container/inventory.png");
            assets.eggModel = LoadModel("./assets/models/egg/scene.gltf");
        });
}
