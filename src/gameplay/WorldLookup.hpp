#pragma once

#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

/// Finds a player entity by id.
flecs::entity findPlayer(flecs::world &world, int id);

/// Finds an egg entity by id.
flecs::entity findEgg(flecs::world &world, int id);

/// Finds a tile entity by coordinates.
flecs::entity findTile(flecs::world &world, int x, int y);

/// Finds a team entity by name.
flecs::entity findTeam(flecs::world &world, const std::string &name);

/// Finds or creates a team entity by name.
flecs::entity findOrCreateTeam(flecs::world &world, const std::string &name);
