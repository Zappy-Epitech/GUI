/**
 * @file Team.hpp
 * @ingroup gui_gameplay
 * @brief Team components and the team registration module.
 */
#pragma once

#include <string>

namespace flecs {
struct world;
}

/// Stores a team name.
struct Team {
    std::string name;
};

/// Marks an entity as belonging to a team.
struct BelongsTo {};

/**
 * @brief Registers team components.
 * @ingroup gui_gameplay
 */
struct Teams {
    /// Imports the team module.
    Teams(flecs::world &world);
};
