#include "PlayerCommand.hpp"
#include "src/extern/flecs.h"
#include "src/minecraft/MinecraftAnimation.hpp"
#include "src/core/Spatial.hpp"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/minecraft/MinecraftRenderer.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include <format>
#include <raylib.h>

static Position3 gridCenterPosition(int x, int y) {
    return Grid::position(x, y).with_y(1.f);
}

void applyPlayerNew(flecs::world &world, zappy::PlayerNew &evt) {
    const GameAssets &skins = world.get<GameAssets>();
    Texture2D skin = skins.skins[evt.id % skins.skins.size()];

    world.entity(std::format("Player({})", evt.id).c_str())
        .set(Player{ .level = evt.level })
        .set(skin)
        .set<Orientation>(evt.orientation)
        .set(gridCenterPosition(evt.x, evt.y))
        .set<Texture2D>(world.get<GameAssets>().skins[0])
        .set<MinecraftSkin>({ .scale = 0.3f })
        .set<zappy::Resources>({})
        .set(Walking);
}

void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt) {
    flecs::entity player = world.lookup(std::format("Player({})", evt.id).c_str());

    player.set(gridCenterPosition(evt.x, evt.y)).set<Orientation>(evt.orientation);
}

void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt) {
    flecs::entity player = world.lookup(std::format("Player({})", evt.id).c_str());

    player.set(Player{ .level = evt.level });
}

void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt) {
    flecs::entity player = world.lookup(std::format("Player({})", evt.id).c_str());

    player
        .set(gridCenterPosition(evt.x, evt.y))
        .set(evt.resources);
}
