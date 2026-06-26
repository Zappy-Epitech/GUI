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
#include <format>
#include <raylib.h>
#include <vector>

namespace {

/// The names of the resources.
static const std::array<const char *, 7> resourceNames = { "food", "linemate", "deraumere", "sibur", "mendiane", "phiras", "thystame" };

struct ResourceVisual {
    Position3 position;
    Rotation3 rotation;
    float size;
};

/// The per-resource transforms normalize heterogeneous GLTF origins and sizes.
/// Position is relative to the tile origin after model rotation and scaling.
static const std::array<ResourceVisual, 7> resourceVisuals = {
    ResourceVisual{ Position3{ 0.00000f, 0.54455f, -0.12000f }, Rotation3{ -90.0f, 0.0f, -30.0f }, 0.12800f }, // food / apple
    ResourceVisual{ Position3{ -0.25000f, 0.47000f, -0.37000f }, Rotation3{ 90.0f, 0.0f, 0.0f }, 0.32000f },   // linemate / emerald
    ResourceVisual{ Position3{ 0.25003f, 0.55470f, -0.37000f }, Rotation3{ 90.0f, 0.0f, 30.0f }, 0.14769f },   // deraumere / coal
    ResourceVisual{ Position3{ -0.24993f, 0.45947f, -0.00030f }, Rotation3{ 90.0f, 0.0f, 0.0f }, 0.04808f },   // sibur / iron
    ResourceVisual{ Position3{ 0.25007f, 0.45947f, -0.00030f }, Rotation3{ 90.0f, 0.0f, 0.0f }, 0.04808f },    // mendiane / gold
    ResourceVisual{ Position3{ -0.23174f, 0.57001f, 0.12982f }, Rotation3{ 90.0f, 0.0f, 90.0f }, 0.02503f },   // phiras / diamond
    ResourceVisual{ Position3{ 0.28956f, 0.01370f, 0.24902f }, Rotation3{ 90.0f, 0.0f, 26.0f }, 0.32929f },    // thystame / redstone
};

/// Views resources as an indexed array.
static const std::array<int, 7> &resourceAmounts(const zappy::Resources &resources) {
    return *reinterpret_cast<const std::array<int, 7> *>(&resources);
}

/// Removes rendered resources from a tile.
static void clearTileResources(flecs::entity tile) {
    for (const char *name : resourceNames) {
        flecs::entity resource = tile.lookup(name);

        if (resource) {
            resource.destruct();
        }
    }
}

struct Linemate {};
struct Deraumere {};
struct Sibure {};
struct Mendiane {};
struct Phiras {};
struct Thystame {};

/// Spawns one rendered resource.
static void spawnTileResource(
    const flecs::world &world,
    flecs::entity tile,
    int resource,
    const Model &model) {
    const Position3 &tilePosition = tile.get<Position3>();
    const ResourceVisual &visual = resourceVisuals[resource];

    world.entity()
        .child_of(tile)
        .set_name(resourceNames[resource])
        .set(Position3{
            tilePosition.x + visual.position.x,
            tilePosition.y + visual.position.y + 0.45f,
            tilePosition.z + visual.position.z,
        })
        .set(model)
        .set(visual.rotation)
        .set(Scale{ visual.size });
}
} // namespace

/// Applies a new map size.
void applyMapNew(const flecs::world &world, zappy::MapSize &evt) {
    Grid::spawn(world, evt.width, evt.height);
}

/// Applies tile resource content.
void applyTileContent(const flecs::world &world, zappy::TileContent &evt) {
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

/// Applies incantation completion.
void applyIncantationEnd(const flecs::world &world, zappy::IncantationEnd &evt) {
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
