#include "Minecraft.hpp"
#include "../../extern/flecs.hpp"

Minecraft::Minecraft(flecs::world &world) {
    flecs::entity module = world.module<Minecraft>();
    world.import<SkinAnim>().child_of(module);
    world.import<MinecraftSkinRenderer>().child_of(module);
}
