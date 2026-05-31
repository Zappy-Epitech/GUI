#pragma once

namespace flecs {
struct world;
}

/// Registers gameplay modules.
struct GamePlay {
    /// Imports gameplay systems.
    GamePlay(flecs::world &world);
};
