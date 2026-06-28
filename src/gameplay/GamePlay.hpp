/**
 * @file GamePlay.hpp
 * @ingroup gui_gameplay
 * @brief Root flecs module aggregating all gameplay sub-modules.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Root flecs module for the gameplay scene.
 * @details Imports and parents the Grid, Teams, Players and Eggs modules,
 *          imports the Movement and (disabled) CameraController modules, and
 *          loads the shared GameAssets singleton. Acts as the parent entity
 *          for every gameplay component and system.
 * @ingroup gui_gameplay
 */
struct GamePlay {
    /// Imports gameplay systems.
    GamePlay(flecs::world &world);
};
