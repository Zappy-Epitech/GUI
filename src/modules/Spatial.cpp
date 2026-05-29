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

    world.component<Rotation3>()
        .member<float>("x")
        .member<float>("y")
        .member<float>("z");

    world.component<Size2>()
        .member<float>("width")
        .member<float>("height");

    world.component<Scale>()
        .member<float>("scale");
}

Position2 Position2::center() {
    return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
}

Position2 Position2::center(float width, float height) {
    return { (WINDOW_WIDTH / 2.0f) - (width / 2.0f), WINDOW_HEIGHT / 2.0f - height / 2.0f };
}

Position2 Position2::bottom_center() {
    return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 100.0f };
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

Position2 Position2::div_x(float x) {
    return { this->x / x, y };
}

Position2 Position2::div_y(float y) {
    return { x, this->y / y };
}

Rectangle Position2::rect(float width, float height) {
    return { x, y, width, height };
}

Position3 Position3::zero() {
    return { 0, 0, 0 };
}

Position3 Position3::from_xyz(float x, float y, float z) {
    return { x, y, z };
}

Position3 Position3::with_x(float x) {
    return { x, y, z };
}

Position3 Position3::with_y(float y) {
    return { x, y, z };
}

Position3 Position3::with_z(float z) {
    return { x, y, z };
}

Position3 Position3::sub_x(float x) {
    return { this->x - x, y, z };
}

Position3 Position3::sub_y(float y) {
    return { x, this->y - y, z };
}

Position3 Position3::sub_z(float z) {
    return { x, y, this->z - z };
}

Position3 Position3::add_x(float x) {
    return { this->x + x, y, z };
}

Position3 Position3::add_y(float y) {
    return { x, this->y + y, z };
}

Position3 Position3::add_z(float z) {
    return { x, y, this->z + z };
}

Rotation3 Rotation3::zero() {
    return { 0, 0, 0 };
}

Rotation3 Rotation3::from_xyz(float x, float y, float z) {
    return { x, y, z };
}
