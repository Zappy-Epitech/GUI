#pragma once
#include "src/modules/SkinAnimation.hpp"
#include "src/modules/rendering/MinecraftSkinRenderer.hpp"

namespace flecs {
struct world;
}

struct Minecraft {
    Minecraft(flecs::world &world);
};
