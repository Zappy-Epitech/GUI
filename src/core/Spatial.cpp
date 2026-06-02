#include "Spatial.hpp"
#include "src/extern/flecs.h"

/// Registers spatial components.
Spatial::Spatial(flecs::world &world) {
    world.module<Spatial>("spatial");

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
        .member<float>("value");

    world.component<Orientation>();
}

/// Returns the screen center.
Position2 Position2::center() {
    return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
}

/// Returns a centered position for a size.
Position2 Position2::center(float width, float height) {
    return { (WINDOW_WIDTH / 2.0f) - (width / 2.0f), WINDOW_HEIGHT / 2.0f - height / 2.0f };
}

/// Returns the bottom center point.
Position2 Position2::bottom_center() {
    return { WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 100.0f };
}

/// Creates a position with equal values.
Position2 Position2::splat(float value) {
    return { value, value };
}

/// Returns a copy with a new x value.
Position2 Position2::with_x(float x) {
    return { x, y };
}

/// Returns a copy with a new y value.
Position2 Position2::with_y(float y) {
    return { x, y };
}

/// Returns a copy shifted left.
Position2 Position2::sub_x(float x) {
    return { this->x - x, y };
}

/// Returns a copy shifted up.
Position2 Position2::sub_y(float y) {
    return { x, this->y - y };
}

/// Returns a copy shifted right.
Position2 Position2::add_x(float x) {
    return { this->x + x, y };
}

/// Returns a copy shifted down.
Position2 Position2::add_y(float y) {
    return { x, this->y + y };
}

/// Returns a copy with divided x.
Position2 Position2::div_x(float x) {
    return { this->x / x, y };
}

/// Returns a copy with divided y.
Position2 Position2::div_y(float y) {
    return { x, this->y / y };
}

/// Builds a rectangle from this position.
Rectangle Position2::rect(float width, float height) {
    return { x, y, width, height };
}

/// Returns the origin position.
Position3 Position3::zero() {
    return { 0, 0, 0 };
}

/// Creates a position from coordinates.
Position3 Position3::from_xyz(float x, float y, float z) {
    return { x, y, z };
}

/// Returns a copy with a new x value.
Position3 Position3::with_x(float x) const {
    return { x, y, z };
}

/// Returns a copy with a new y value.
Position3 Position3::with_y(float y) const {
    return { x, y, z };
}

/// Returns a copy with a new z value.
Position3 Position3::with_z(float z) const {
    return { x, y, z };
}

/// Returns a copy shifted on x.
Position3 Position3::sub_x(float x) const {
    return { this->x - x, y, z };
}

/// Returns a copy shifted down.
Position3 Position3::sub_y(float y) const {
    return { x, this->y - y, z };
}

/// Returns a copy shifted on z.
Position3 Position3::sub_z(float z) const {
    return { x, y, this->z - z };
}

/// Returns a copy advanced on x.
Position3 Position3::add_x(float x) const {
    return { this->x + x, y, z };
}

/// Returns a copy advanced on y.
Position3 Position3::add_y(float y) const {
    return { x, this->y + y, z };
}

/// Returns a copy advanced on z.
Position3 Position3::add_z(float z) const {
    return { x, y, this->z + z };
}

/// Returns a zero rotation.
Rotation3 Rotation3::zero() {
    return { 0, 0, 0 };
}

/// Creates a rotation from angles.
Rotation3 Rotation3::from_xyz(float x, float y, float z) {
    return { x, y, z };
}
