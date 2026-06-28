/**
 * @file WorldLookup.hpp
 * @ingroup gui_gameplay
 * @brief Helpers to look up gameplay entities by id, coordinates or name.
 */
#pragma once

#include <string>

namespace flecs {
struct world;
struct entity;
} // namespace flecs

/**
 * @brief Finds a player entity by its server id.
 * @param world ECS world to query.
 * @param id Server player id to match.
 * @return The matching player entity, or an invalid (falsy) flecs::entity if no
 *         player carries that id.
 */
flecs::entity findPlayer(const flecs::world &world, int id);

/**
 * @brief Finds an egg entity by its server id.
 * @param world ECS world to query.
 * @param id Server egg id to match.
 * @return The matching egg entity, or an invalid (falsy) flecs::entity if no
 *         egg carries that id.
 */
flecs::entity findEgg(const flecs::world &world, int id);

/**
 * @brief Finds a grid tile entity by its coordinates.
 * @param world ECS world to query.
 * @param x Tile column index.
 * @param y Tile row index.
 * @return The matching tile entity, or an invalid (falsy) flecs::entity if no
 *         tile exists at those coordinates.
 */
flecs::entity findTile(const flecs::world &world, int x, int y);

/**
 * @brief Finds a team entity by name.
 * @param world ECS world to query.
 * @param name Team name to match.
 * @return The matching team entity, or an invalid (falsy) flecs::entity if no
 *         team has that name.
 */
flecs::entity findTeam(const flecs::world &world, const std::string &name);

/**
 * @brief Finds a team entity by name, creating it if absent.
 * @param world ECS world to query and, if needed, spawn into.
 * @param name Team name to match or create.
 * @return The existing team entity, or a newly created, always-valid team
 *         entity parented to the Teams module.
 */
flecs::entity findOrCreateTeam(const flecs::world &world, const std::string &name);
