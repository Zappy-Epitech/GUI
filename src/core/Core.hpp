#pragma once

namespace flecs {
struct world;
}

/// Marks an entity for timed destruction.
struct Lifetime {
    float remaining = 0.0f;
};

/// Registers the core ECS module.
struct Core {
    /// Imports core systems and components.
    Core(flecs::world &world);
};
