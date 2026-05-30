#include "MapCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GameAssets.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <format>
#include <raylib.h>
#include <vector>

namespace {

static const std::array<const char *, 7> resourceNames = { "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame" };

static const std::array<Position3, 7> resourceOffsets = {
    Position3{ 0.00f, 0.00f, 0.00f },
    Position3{ -0.25f, 0.00f, -0.25f },
    Position3{ 0.25f, 0.00f, -0.25f },
    Position3{ -0.25f, 0.00f, 0.00f },
    Position3{ 0.25f, 0.00f, 0.00f },
    Position3{ -0.25f, 0.00f, 0.25f },
    Position3{ 0.25f, 0.00f, 0.25f },
};

static const std::array<int, 7> &resourceAmounts(const zappy::Resources &resources) {
    return *reinterpret_cast<const std::array<int, 7> *>(&resources);
}

static void clearTileResources(flecs::entity tile) {
    for (const char *name : resourceNames) {
        flecs::entity resource = tile.lookup(name);

        if (resource) {
            resource.destruct();
        }
    }
}

static void spawnTileResource(
    flecs::world &world,
    flecs::entity tile,
    int resource,
    const Model &model) {
    Position3 offset = resourceOffsets[resource];

    world.entity()
        .child_of(tile)
        .set_name(resourceNames[resource])
        .set(Position3{ offset.x, 0.45f + offset.y, offset.z })
        .set(model)
        .set(Rotation3::from_xyz(90, 0, 0))
        .set(Scale{ 0.83f });
}

} // namespace

void applyMapNew(flecs::world &world, zappy::MapSize &evt) {
    world.query_builder()
        .with<GridContainer>()
        .build()
        .each([](flecs::entity e) {
            e.destruct();
        });

    Grid::spawn(world, evt.width, evt.height);
}

void applyTileContent(flecs::world &world, zappy::TileContent &evt) {
    flecs::entity tile = findTile(world, evt.x, evt.y);

    if (!tile) {
        return;
    }

    const GameAssets &assets = world.get<GameAssets>();
    const std::array<int, 7> &amounts = resourceAmounts(evt.resources);

    tile.set(evt.resources);
    clearTileResources(tile);

    for (int resourceIndex = 0; resourceIndex < static_cast<int>(amounts.size()); resourceIndex++) {
        if (amounts[resourceIndex] > 0) {
            spawnTileResource(world, tile, resourceIndex, assets.resourceModels[resourceIndex]);
        }
    }
}

void applyIncantationEnd(flecs::world &world, zappy::IncantationEnd &evt) {
    flecs::entity tile = findTile(world, evt.x, evt.y);

    if (!tile) {
        return;
    }

    world.entity()
        .set(ScreenMessage{ std::format("Incantation at ({}, {}) {}", evt.x, evt.y, evt.success ? "succeeded" : "failed") })
        .set<Color>(evt.success ? GREEN : RED)
        .set(Lifetime{ 2.0f });

    Position3 tilePosition = Grid::position(evt.x, evt.y).with_y(1.0f);
    std::vector<flecs::entity> incantatingPlayers;

    world.query_builder<const Player, const Position3>()
        .with<Incantating>()
        .build()
        .each([&](flecs::entity player, const Player &, const Position3 &position) {
            if (std::abs(position.x - tilePosition.x) < 0.01f && std::abs(position.z - tilePosition.z) < 0.01f) {
                incantatingPlayers.push_back(player);
            }
        });

    for (flecs::entity player : incantatingPlayers) {
        player.remove<Incantating>();
    }
}
