#pragma once

#include <raylib.h>

namespace flecs {
struct world;
}

struct CameraController {
    CameraController(flecs::world &world);

    static Camera &camera();
};
