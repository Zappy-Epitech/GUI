#pragma once

namespace flecs {
struct world;
struct entity;
} // namespace flecs

flecs::entity findPlayer(flecs::world &world, int id);
flecs::entity findTile(flecs::world &world, int x, int y);
