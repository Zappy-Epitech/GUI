#pragma once

static constexpr float WINDOW_WIDTH = 1920.0f;
static constexpr float WINDOW_HEIGHT = 1080.0f;

namespace flecs {
struct world;
}

struct Position2 {
    float x, y;

    static Position2 center();
    static Position2 splat(float value);
    Position2 with_x(float x);
    Position2 with_y(float y);
    Position2 sub_x(float x);
    Position2 sub_y(float y);
    Position2 add_x(float x);
    Position2 add_y(float y);
};
struct Position3 {
    float x, y, z;
};

struct Size2 {
    float width, height;
};

struct Spatial {
    Spatial(flecs::world &world);
};
