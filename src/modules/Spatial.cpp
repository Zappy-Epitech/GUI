#include "Spatial.hpp"
#include "../extern/flecs.hpp"

Spatial::Spatial(flecs::world &world) {
    world.module<Spatial>("Spatial");

    world.component<Position2>()
        .member<float>("x")
        .member<float>("y");

    world.component<Position3>()
        .member<float>("x")
        .member<float>("y")
        .member<float>("z");

    world.component<Size2>()
        .member<float>("width")
        .member<float>("height");
}

Position2 Position2::center() {
    return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
}

Position2 Position2::splat(float value) {
    return { value, value };
}

Position2 Position2::with_x(float x) {
    return { x, y };
}

Position2 Position2::with_y(float y) {
    return { x, y };
}

Position2 Position2::sub_x(float x) {
    return { this->x - x, y };
}

Position2 Position2::sub_y(float y) {
    return { x, this->y - y };
}

Position2 Position2::add_x(float x) {
    return { this->x + x, y };
}

Position2 Position2::add_y(float y) {
    return { x, this->y + y };
}
