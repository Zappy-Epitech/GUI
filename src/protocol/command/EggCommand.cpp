#include "EggCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
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
    return Grid::position(x, y).add_y(0.1f);
}

/// Adds a temporary screen message.
static void addScreenMessage(flecs::world &world, const std::string &text, Color color, float lifetime) {
    world.entity()
        .set(ScreenMessage{ text })
        .set<Color>(color)
        .set(Lifetime{ lifetime });
}

/// Removes a laying preview for a player if present.
static void clearEggPreview(flecs::world &world, int playerId) {
    if (flecs::entity preview = world.lookup(std::format("EggPreview({})", playerId).c_str()); preview) {
        preview.destruct();
    }
}

} // namespace

/// Applies a new egg event.
void applyEggNew(flecs::world &world, zappy::EggNew &evt) {
    clearEggPreview(world, evt.playerId);

    if (flecs::entity existing = findEgg(world, evt.id); existing) {
        existing.destruct();
    }

    const GameAssets &assets = world.get<GameAssets>();

    world.entity(std::format("Egg({})", evt.id).c_str())
        .child_of<Game>()
        .set(EggId{ evt.id })
        .set(eggPosition(evt.x, evt.y))
        .set(Rotation3::from_xyz(90, 0, 0))
        .set(assets.eggModel)
        .set(Scale{ 0.07f });

    addScreenMessage(
        world,
        std::format("Egg #{} laid at ({}, {}) by player #{}", evt.id, evt.x, evt.y, evt.playerId),
        WHITE,
        2.0f);
}

/// Applies an egg hatched event.
void applyEggHatched(flecs::world &world, zappy::EggHatched &evt) {
    if (flecs::entity egg = findEgg(world, evt.id); egg) {
        egg.destruct();
    }

    addScreenMessage(world, std::format("Egg #{} hatched", evt.id), GREEN, 2.0f);
}

/// Applies an egg death event.
void applyEggDeath(flecs::world &world, zappy::EggDeath &evt) {
    if (flecs::entity egg = findEgg(world, evt.id); egg) {
        egg.destruct();
    }

    addScreenMessage(world, std::format("Egg #{} died", evt.id), RED, 2.0f);
}
