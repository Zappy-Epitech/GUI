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
            assets.eggModel = LoadModel("./assets/models/egg/scene.gltf");
        });
}
