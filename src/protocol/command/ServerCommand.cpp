#include "ServerCommand.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Simulation.hpp"
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

} // namespace

/// Applies a time unit event.
void applyTimeUnit(const flecs::world &world, zappy::TimeUnit &evt) {
    world.set<SimulationTime>({ evt.value });

    addScreenMessage(world, std::format("Time unit set to {}", evt.value), SKYBLUE, 2.0f);
}

/// Applies a game end event.
void applyGameEnd(const flecs::world &world, zappy::GameEnd &evt) {
    world.set<GameResult>({ true, evt.winner });

    addScreenMessage(world, std::format("Team {} wins!", evt.winner), GOLD, 10.0f);
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
