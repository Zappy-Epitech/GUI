#pragma once

namespace flecs {
struct world;
}

/// Registers the game scene.
struct Game {
    /// Imports the game scene.
    Game(flecs::world &world);
};
