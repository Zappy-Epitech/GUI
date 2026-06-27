#include "Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/IncantationEffect.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Settings.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"

/// Imports core modules and lifetime cleanup.
Core::Core(flecs::world &world) {
    flecs::entity module = world.module<Core>("core");
    world.import<Spatial>().child_of(module);
    world.import<Raylib>().child_of(module);
    world.import<Settings>().child_of(module);
    world.import<IncantationEffects>().child_of(module);
    world.import<Gui>().child_of(module);
    world.import<Scenes>().child_of(module);

    world.component<Lifetime>();

    world.system<Lifetime>("LifetimeSystem")
        .kind(flecs::PostUpdate)
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto lifetimes = it.field<Lifetime>(0);

                for (auto i : it) {
                    lifetimes[i].remaining -= it.delta_time();
                    if (lifetimes[i].remaining <= 0.0f) {
                        it.entity(i).destruct();
                    }
                }
            }
        });
}
