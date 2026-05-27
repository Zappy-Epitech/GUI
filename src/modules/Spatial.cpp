#include "Spatial.hpp"
#include "../extern/flecs.hpp"

Spatial::Spatial(flecs::world &world) {
    world.module<Spatial>("Spatial");

    world.component<Position2>()
        .member<float>("x")
        .member<float>("y");

    world.component<Size2>()
        .member<float>("width")
        .member<float>("height");
}
