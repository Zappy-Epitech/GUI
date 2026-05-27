#pragma once
#include "Spatial.hpp"

namespace flecs {
struct world;
}

struct Cube {};
struct Render3D {};
struct Render2D {};
struct PostRender {};

struct Raylib {
    Raylib(flecs::world &world);
};
