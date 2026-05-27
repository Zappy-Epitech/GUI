#pragma once

static constexpr float WINDOW_WIDTH = 1920.0f;
static constexpr float WINDOW_HEIGHT = 1080.0f;

namespace flecs {
struct world;
}

struct Position2 {
    float x, y;

    static Position2 center() {
        return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
    }
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
