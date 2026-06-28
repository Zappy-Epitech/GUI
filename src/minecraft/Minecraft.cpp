/**
 * @file Minecraft.cpp
 * @ingroup gui_minecraft
 * @brief Implements the top-level Minecraft module by importing its child modules.
 */
#include "Minecraft.hpp"
#include "src/extern/flecs.h"
#include "src/minecraft/MinecraftAnimation.hpp"
#include "src/minecraft/MinecraftRenderer.hpp"

/// Imports Minecraft rendering modules.
Minecraft::Minecraft(flecs::world &world) {
    flecs::entity mod = world.module<Minecraft>("minecraft");
    world.import<MinecraftAnimation>().child_of(mod);
    world.import<MinecraftRenderer>().child_of(mod);
}
