/**
 * @file ServerCommand.hpp
 * @ingroup gui_protocol
 * @brief Handlers that apply server-level protocol events to the world.
 */
#pragma once
#include "../ZappyProtocol.hpp"

namespace flecs {
struct world;
}

/// Applies a time unit event.
void applyTimeUnit(const flecs::world &world, zappy::TimeUnit &evt);

/// Applies a game end event.
void applyGameEnd(const flecs::world &world, zappy::GameEnd &evt);

/// Applies a server message event.
void applyServerMessage(const flecs::world &world, zappy::ServerMessage &evt);

/// Applies an unknown command event.
void applyUnknownCommand(const flecs::world &world, zappy::UnknownCommand &evt);

/// Applies a bad parameter event.
void applyBadParameter(const flecs::world &world, zappy::BadParameter &evt);
