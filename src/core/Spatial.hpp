#pragma once
#include <raylib.h>

static constexpr float WINDOW_WIDTH = 1920.0f;
static constexpr float WINDOW_HEIGHT = 1200.0f;

namespace flecs {
struct world;
}

struct Position2 {
    float x, y;

    static Position2 center();
    static Position2 center(float width, float height);
    static Position2 bottom_center();
    static Position2 splat(float value);

    Position2 with_x(float x);
    Position2 with_y(float y);
    Position2 sub_x(float x);
    Position2 sub_y(float y);
    Position2 add_x(float x);
    Position2 add_y(float y);
    Position2 div_x(float x);
    Position2 div_y(float y);

    Rectangle rect(float width, float height);
};
struct Position3 {
    float x, y, z;

    static Position3 zero();
    static Position3 from_xyz(float x, float y, float z);
    Position3 with_x(float x) const;
    Position3 with_y(float y) const;
    Position3 with_z(float z) const;
    Position3 sub_x(float x) const;
    Position3 sub_y(float y) const;
    Position3 sub_z(float z) const;
    Position3 add_x(float x) const;
    Position3 add_y(float y) const;
    Position3 add_z(float z) const;
};

struct Rotation3 {
    float x, y, z;

    static Rotation3 zero();
    static Rotation3 from_xyz(float x, float y, float z);
};

enum struct Orientation : int {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4,
};

struct Size2 {
    float width, height;
};

struct Scale {
    float value;
};

struct Spatial {
    Spatial(flecs::world &world);
};
