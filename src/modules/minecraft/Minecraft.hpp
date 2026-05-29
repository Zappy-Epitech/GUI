#pragma once
#include "src/extern/flecs.hpp"
#include "src/modules/SkinAnimation.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"

struct Minecraft {
    Minecraft(flecs::world &world) {
        flecs::entity module = world.module<Minecraft>();
        world.import<SkinAnim>().child_of(module);
        world.import<MinecraftSkinRenderer>().child_of(module);
    }
};
