#pragma once

namespace flecs {
struct world;
}

/// Registers the settings scene.
struct SettingsScene {
    /// Imports the settings scene.
    SettingsScene(flecs::world &world);
};
