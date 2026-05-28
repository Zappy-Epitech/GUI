#pragma once


#include "src/extern/flecs.hpp"
#include <filesystem>
#include <raylib.h>
#include <vector>

struct GameAssets {
    std::vector<Texture2D> skins;

    static void spawn(flecs::world &world) {
        world.singleton<GameAssets>();
        world.set<GameAssets>({});

        world.system<GameAssets>("LoadAssets")
            .term_at(0)
            .self()
            .kind(flecs::OnStart)
            .each([](GameAssets &assets) {
                for (const auto &file : std::filesystem::directory_iterator("./assets/skins/")) {
                    Texture2D texture = LoadTexture(file.path().c_str());
                    assets.skins.push_back(texture);
                }
            });
    }
};
