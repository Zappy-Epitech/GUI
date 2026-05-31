#include "GameAssets.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include <filesystem>

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
            for (const auto &file : std::filesystem::directory_iterator("./assets/skins/")) {
                Texture2D texture = LoadTexture(file.path().c_str());
                assets.skins.push_back(texture);
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
