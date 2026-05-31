#pragma once
#include <raylib.h>

namespace flecs {
struct world;
}

/// Controls the shared gameplay camera.
struct CameraController {
    /// Registers camera control systems.
    CameraController(flecs::world &world);

    /// Returns the active camera.
    static Camera &camera();
};
