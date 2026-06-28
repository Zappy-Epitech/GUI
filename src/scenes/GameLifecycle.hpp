/**
 * @file GameLifecycle.hpp
 * @ingroup gui_scenes
 * @brief flecs module handling enter/exit transitions of the game scene.
 */
#pragma once

namespace flecs {
struct world;
}

/**
 * @brief Registers game scene lifecycle observers.
 * @details On entering the Game scene it resets GameUiState, enables the
 * CameraController and spawns the Exit and Settings buttons; on exit it
 * disables the CameraController.
 * @ingroup gui_scenes
 */
struct GameLifecycle {
    /// Imports the game lifecycle module.
    GameLifecycle(flecs::world &world);
};
