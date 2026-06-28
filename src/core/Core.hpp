/**
 * @file Core.hpp
 * @ingroup gui_core
 * @brief Root ECS module that aggregates the core sub-modules and the Lifetime component.
 */
#pragma once

namespace flecs {
struct world;
}

/// Marks an entity for timed destruction.
struct Lifetime {
    float remaining = 0.0f; ///< Seconds left before the entity is destroyed.
};

/**
 * @brief Root ECS module of the GUI client.
 * @details Imports the Spatial, Raylib, Settings, IncantationEffects, Gui and
 * Scenes sub-modules, registers the Lifetime component, and runs the
 * LifetimeSystem that destroys entities once their Lifetime expires.
 * @ingroup gui_core
 */
struct Core {
    /// Imports core systems and components.
    Core(flecs::world &world);
};
