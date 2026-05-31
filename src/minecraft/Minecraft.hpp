#pragma once

namespace flecs {
struct world;
}

/// Registers Minecraft-style rendering modules.
struct Minecraft {
    /// Imports Minecraft modules.
    Minecraft(flecs::world &world);
};
