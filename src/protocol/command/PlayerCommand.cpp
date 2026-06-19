#include "PlayerCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Scenes.hpp"
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

/// The names of the resources.
static const std::array<const char *, 7> resourceNames = { "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame" };

/// The colors of the resources.
static const std::array<Color, 7> resourceColors = { LIME, GREEN, SKYBLUE, BLUE, PURPLE, ORANGE, GOLD };

/// Returns the player center position on a tile.
static Position3 gridCenterPosition(int x, int y) {
    return Grid::position(x, y).with_y(1.f);
}

/// Adds a temporary screen message.
static void addScreenMessage(const flecs::world &world, const std::string &text, Color color, float lifetime) {
    world.entity()
        .set(ScreenMessage{ text })
        .set<Color>(color)
        .set(Lifetime{ lifetime })
        .add<DespawnOnExit>(sceneId<Game>(world));
}

} // namespace

/// Applies a new player event.
void applyPlayerNew(const flecs::world &world, const zappy::PlayerNew &evt) {
    const GameAssets &skins = world.get<GameAssets>();
    Texture2D skin = skins.skins[evt.id % skins.skins.size()];
    flecs::entity team = findOrCreateTeam(world, evt.team);

    flecs::entity player = findPlayer(world, evt.id);

    if (!player) {
        player = world.entity(std::format("Player({})", evt.id).c_str());
    }

    player
        .add<BelongsTo>(team)
        .set(PlayerId{ evt.id })
        .set(Player{ .level = evt.level })
        .set(skin)
        .set(Rotation3::zero())
        .set<Orientation>(evt.orientation)
        .set(gridCenterPosition(evt.x, evt.y))
        .set<Texture2D>(world.get<GameAssets>().skins[0])
        .set<MinecraftSkin>({ .scale = 0.35f })
        .set<zappy::Resources>({})
        .set(Walking)
        .add<DespawnOnExit>(sceneId<Game>(world));
}

/// Applies a player position event.
void applyPlayerPosition(const flecs::world &world, zappy::PlayerPosition &evt) {
    float rotation_y = evt.orientation == Orientation::NORTH ? 0 : evt.orientation == Orientation::SOUTH ? 180
                                                               : evt.orientation == Orientation::EAST    ? 90
                                                                                                         : 270;
    findPlayer(world, evt.id).set(Direction(gridCenterPosition(evt.x, evt.y))).set<Orientation>(evt.orientation).set(Rotation3::from_xyz(0, rotation_y, 0));
}

/// Applies a player level event.
void applyPlayerLevel(const flecs::world &world, zappy::PlayerLevel &evt) {
    findPlayer(world, evt.id)
        .set(Player{ .level = evt.level });
}

/// Applies a player inventory event.
void applyPlayerInventory(const flecs::world &world, zappy::PlayerInventory &evt) {
    findPlayer(world, evt.id)
        .set(gridCenterPosition(evt.x, evt.y))
        .set(evt.resources);
}

/// Applies a player expulsion event.
void applyPlayerExpelled(const flecs::world &world, zappy::PlayerExpelled &evt) {
    addScreenMessage(world, std::format("Player #{} was expelled", evt.id), ORANGE, 2.0f);
}

/// Applies a player broadcast event.
void applyPlayerBroadcast(const flecs::world &world, zappy::PlayerBroadcast &evt) {
    addScreenMessage(world, std::format("Player #{}: {}", evt.id, evt.message), WHITE, 4.0f);
}

/// Applies an incantation start event.
void applyIncantationStart(const flecs::world &world, zappy::IncantationStart &evt) {
    for (int id : evt.playerIds) {
        flecs::entity player = findPlayer(world, id);

        if (player) {
            player.add<Incantating>();
        }
    }

    addScreenMessage(
        world,
        std::format("Incantation level {} at ({}, {})", evt.level, evt.x, evt.y),
        YELLOW,
        2.0f);
}

/// Applies an egg laying start event.
void applyPlayerEggLayStart(const flecs::world &world, zappy::PlayerEggLayStart &evt) {
    world.entity(std::format("EggPreview({})", evt.id).c_str())
        .set(findPlayer(world, evt.id).get<Position3>().add_y(0.1))
        .set(Rotation3::from_xyz(90, 0, 0))
        .set(world.get<GameAssets>().eggModel)
        .set(Scale{ 0.07f })
        .set(Lifetime{ 3.0f })
        .add<DespawnOnExit>(sceneId<Game>(world));

    addScreenMessage(world, std::format("Player #{} is laying an egg", evt.id), WHITE, 2.0f);
}

/// Applies a player resource drop event.
void applyPlayerResourceDrop(const flecs::world &world, zappy::PlayerResourceDrop &evt) {
    addScreenMessage(
        world,
        std::format("Player #{} dropped {}", evt.id, resourceNames[evt.resource]),
        resourceColors[evt.resource],
        2.0f);
}

/// Applies a player resource collect event.
void applyPlayerResourceCollect(const flecs::world &world, zappy::PlayerResourceCollect &evt) {
    addScreenMessage(
        world,
        std::format("Player #{} collected {}", evt.id, resourceNames[evt.resource]),
        resourceColors[evt.resource],
        2.0f);
}

/// Applies a player death event.
void applyPlayerDeath(const flecs::world &world, zappy::PlayerDeath &evt) {
    flecs::entity player = findPlayer(world, evt.id);

    if (player) {
        player.destruct();
    }

    addScreenMessage(world, std::format("Player #{} died", evt.id), RED, 2.0f);
}
