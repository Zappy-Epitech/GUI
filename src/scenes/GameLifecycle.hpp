#pragma once

namespace flecs {
struct world;
}

/// Registers game scene lifecycle observers.
struct GameLifecycle {
    /// Imports the game lifecycle module.
    GameLifecycle(flecs::world &world);
};
