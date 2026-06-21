#include "ServerCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Scenes.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/Team.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/scenes/EndGame.hpp"
#include <format>
#include <raylib.h>
#include <string>

namespace {

/// Adds a temporary screen message.
static void addScreenMessage(const flecs::world &world, const std::string &text, Color color, float lifetime) {
    world.entity()
        .set(ScreenMessage{ text })
        .set<Color>(color)
        .set(Lifetime{ lifetime });
}

static EndGameState buildEndGameState(const flecs::world &world, const std::string &winner) {
    EndGameState state;
    state.winner = winner;

    flecs::entity team = findTeam(world, winner);
    if (!team) {
        return state;
    }

    world.query_builder<const Player, const PlayerId, const zappy::Resources, const Texture2D>()
        .build()
        .each([&](flecs::entity entity, const Player &player, const PlayerId &playerId, const zappy::Resources &resources, const Texture2D &skin) {
            if (!entity.has<BelongsTo>(team)) {
                return;
            }

            state.players.push_back(EndGamePlayerStats{
                .name = entity.name().c_str(),
                .id = playerId.value,
                .level = player.level,
                .resources = resources,
                .skin = skin,
            });
        });

    return state;
}

} // namespace

/// Applies a time unit event.
void applyTimeUnit(const flecs::world &world, zappy::TimeUnit &evt) {
    world.set<SimulationTime>({ evt.value });

    addScreenMessage(world, std::format("Time unit set to {}", evt.value), SKYBLUE, 2.0f);
}

/// Applies a game end event.
void applyGameEnd(const flecs::world &world, zappy::GameEnd &evt) {
    world.set<GameResult>({ true, evt.winner });
    world.set<EndGameState>(buildEndGameState(world, evt.winner));

    flecs::world mutableWorld = world;
    enterScene<EndGame>(mutableWorld);
}

/// Applies a server message event.
void applyServerMessage(const flecs::world &world, zappy::ServerMessage &evt) {
    addScreenMessage(world, std::format("Server: {}", evt.message), SKYBLUE, 5.0f);
}

/// Applies an unknown command event.
void applyUnknownCommand(const flecs::world &world, zappy::UnknownCommand &) {
    addScreenMessage(world, "Server: unknown command", ORANGE, 3.0f);
}

/// Applies a bad parameter event.
void applyBadParameter(const flecs::world &world, zappy::BadParameter &) {
    addScreenMessage(world, "Server: bad parameter", RED, 3.0f);
}
