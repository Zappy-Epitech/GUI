#pragma once

namespace flecs {
struct world;
}

/// Stores Minecraft skin render settings.
struct MinecraftSkin {
    float scale = 1.0f;
};

/// Registers Minecraft skin rendering.
struct MinecraftRenderer {
    /// Imports the renderer module.
    MinecraftRenderer(flecs::world &world);
};
