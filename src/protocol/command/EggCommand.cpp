#include "EggCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Egg.hpp"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/scenes/Game.hpp"
#include <format>
#include <raylib.h>
#include <string>

namespace {

/// Returns an egg position on a tile.
static Position3 eggPosition(int x, int y) {
    return Grid::position(x, y).add_y(0.83);
}

/// Adds a temporary screen message.
static void addScreenMessage(const flecs::world &world, const std::string &text, Color color, float lifetime) {
    world.entity()
        .set(ScreenMessage{ text })
        .set<Color>(color)
        .set(Lifetime{ lifetime });
}
} // namespace

/// Applies a new egg event.
void applyEggNew(const flecs::world &world, zappy::EggNew &evt) {
    if (flecs::entity existing = findEgg(world, evt.id); existing) {
        existing.destruct();
    }

    const GameAssets &assets = world.get<GameAssets>();

    world.entity(std::format("Egg({})", evt.id).c_str())
        .child_of<Game>()
        .set(EggId{ evt.id })
        .set(eggPosition(evt.x, evt.y).add_y(-0.18f))
        .set(Rotation3::from_xyz(90, 0, 0))
        .set(assets.eggModel)
        .set(Scale{ 0.0f })
        .add<DespawnOnExit>(sceneId<Game>(world))
        .set(EggSpawnAnimation{ eggPosition(evt.x, evt.y), 0.0f, 0.22f });

    addScreenMessage(
        world,
        std::format("Egg #{} laid at ({}, {}) by player #{}", evt.id, evt.x, evt.y, evt.playerId),
        WHITE,
        2.0f);
}

/// Applies an egg hatched event.
void applyEggHatched(const flecs::world &world, zappy::EggHatched &evt) {
    if (flecs::entity egg = findEgg(world, evt.id); egg) {
        egg.destruct();
    }

    addScreenMessage(world, std::format("Egg #{} hatched", evt.id), GREEN, 2.0f);
}

/// Applies an egg death event.
void applyEggDeath(const flecs::world &world, zappy::EggDeath &evt) {
    if (flecs::entity egg = findEgg(world, evt.id); egg) {
        egg.destruct();
    }

    addScreenMessage(world, std::format("Egg #{} died", evt.id), RED, 2.0f);
}
