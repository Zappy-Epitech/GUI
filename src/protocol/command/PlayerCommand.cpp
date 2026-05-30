#include "PlayerCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Movement.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/minecraft/MinecraftAnimation.hpp"
#include "src/minecraft/MinecraftRenderer.hpp"
#include "src/protocol/ZappyProtocol.hpp"
#include "src/scenes/Game.hpp"
#include <array>
#include <format>
#include <raylib.h>
#include <string>

namespace {

static const std::array<const char *, 7> resourceNames = { "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame" };
static const std::array<Color, 7> resourceColors = { LIME, GREEN, SKYBLUE, BLUE, PURPLE, ORANGE, GOLD };

static Position3 gridCenterPosition(int x, int y) {
    return Grid::position(x, y).with_y(1.f);
}

static void addScreenMessage(flecs::world &world, const std::string &text, Color color, float lifetime) {
    world.entity()
        .set(ScreenMessage{ text })
        .set<Color>(color)
        .set(Lifetime{ lifetime });
}

} // namespace

void applyPlayerNew(flecs::world &world, const zappy::PlayerNew &evt) {
    const GameAssets &skins = world.get<GameAssets>();
    Texture2D skin = skins.skins[evt.id % skins.skins.size()];
    flecs::entity team = findOrCreateTeam(world, evt.team);

    world.entity(std::format("Player({})", evt.id).c_str())
        .child_of<Game>()
        .add<BelongsTo>(team)
        .set(PlayerId{ evt.id })
        .set(Player{ .level = evt.level })
        .set(skin)
        .set<Orientation>(evt.orientation)
        .set(gridCenterPosition(evt.x, evt.y))
        .set<Texture2D>(world.get<GameAssets>().skins[0])
        .set<MinecraftSkin>({ .scale = 0.3f })
        .set<zappy::Resources>({})
        .set(Direction{ 5, 5 })
        .set(Walking);
}

void applyPlayerPosition(flecs::world &world, zappy::PlayerPosition &evt) {
    flecs::entity player = findPlayer(world, evt.id);

    player.set(gridCenterPosition(evt.x, evt.y)).set<Orientation>(evt.orientation);
}

void applyPlayerLevel(flecs::world &world, zappy::PlayerLevel &evt) {
    flecs::entity player = findPlayer(world, evt.id);

    player.set(Player{ .level = evt.level });
}

void applyPlayerInventory(flecs::world &world, zappy::PlayerInventory &evt) {
    flecs::entity player = findPlayer(world, evt.id);

    player
        .set(gridCenterPosition(evt.x, evt.y))
        .set(evt.resources);
}

void applyPlayerBroadcast(flecs::world &world, zappy::PlayerBroadcast &evt) {
    addScreenMessage(world, std::format("Player #{}: {}", evt.id, evt.message), WHITE, 4.0f);
}

void applyPlayerResourceDrop(flecs::world &world, zappy::PlayerResourceDrop &evt) {
    if (evt.resource < 0 || evt.resource >= static_cast<int>(resourceNames.size())) {
        return;
    }

    addScreenMessage(
        world,
        std::format("Player #{} dropped {}", evt.id, resourceNames[evt.resource]),
        resourceColors[evt.resource],
        2.0f);
}
