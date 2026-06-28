/**
 * @file CameraController.hpp
 * @ingroup gui_gameplay
 * @brief Free-fly camera controller for the gameplay scene.
 */
#pragma once
#include <raylib.h>

namespace flecs {
struct world;
}

/**
 * @brief Controls the shared gameplay camera.
 * @ingroup gui_gameplay
 */
struct CameraController {
    /// Registers camera control systems.
    CameraController(flecs::world &world);

    /// Returns the active camera.
    static Camera &camera();
};
