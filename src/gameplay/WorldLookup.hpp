#pragma once

#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

flecs::entity findPlayer(flecs::world &world, int id);
flecs::entity findTile(flecs::world &world, int x, int y);
flecs::entity findTeam(flecs::world &world, const std::string &name);
flecs::entity findOrCreateTeam(flecs::world &world, const std::string &name);
