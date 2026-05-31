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

/// Registers team components.
struct Teams {
    /// Imports the team module.
    Teams(flecs::world &world);
};
