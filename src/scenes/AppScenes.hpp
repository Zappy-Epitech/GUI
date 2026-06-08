#pragma once

namespace flecs {
struct world;
}

/// Registers the concrete application scenes.
struct AppScenes {
    /// Imports scene modules and selects the initial scene.
    AppScenes(flecs::world &world);
};
