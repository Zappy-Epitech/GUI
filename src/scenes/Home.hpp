#pragma once

namespace flecs {
struct world;
}

/// Registers the home scene.
struct Home {
    /// Imports the home scene.
    Home(flecs::world &world);
};
