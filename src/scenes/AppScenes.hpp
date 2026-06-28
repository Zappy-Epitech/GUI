/**
 * @file AppScenes.hpp
 * @ingroup gui_scenes
 * @brief Top-level flecs module that imports every application scene.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Registers the concrete application scenes.
 * @ingroup gui_scenes
 */
struct AppScenes {
    /// Imports scene modules and selects the initial scene.
    AppScenes(flecs::world &world);
};
