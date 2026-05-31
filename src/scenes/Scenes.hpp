#pragma once

namespace flecs {
struct world;
}

/// Registers scene modules.
struct Scenes {
    /// Imports the initial scene.
    Scenes(flecs::world &world);
};
