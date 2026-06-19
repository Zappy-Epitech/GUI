#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a time unit event.
void applyTimeUnit(flecs::world &world, zappy::TimeUnit &evt);

/// Applies a game end event.
void applyGameEnd(flecs::world &world, zappy::GameEnd &evt);

/// Applies a server message event.
void applyServerMessage(flecs::world &world, zappy::ServerMessage &evt);

/// Applies an unknown command event.
void applyUnknownCommand(flecs::world &world, zappy::UnknownCommand &evt);

/// Applies a bad parameter event.
void applyBadParameter(flecs::world &world, zappy::BadParameter &evt);
