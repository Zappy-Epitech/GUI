#include "Raylib.hpp"
#include "../extern/flecs.hpp"
#include <cstdio>
#include <raylib.h>

Raylib::Raylib(flecs::world &world) {
    world.module<Raylib>();

    world.system("Setup Window").kind(flecs::OnStart).run([](flecs::iter &) {
        InitWindow(1920, 1080, "Zappy");
    });

    world.system("Begin Frame").kind(flecs::PreUpdate).run([](flecs::iter &it) {
        if (WindowShouldClose()) {
            it.world().quit();
        } else {
            BeginDrawing();
            ClearBackground(BLACK);
        }
    });

    world.system("End Frame").kind(flecs::PostUpdate).run([](flecs::iter &it) {
        if (IsWindowReady()) {
            EndDrawing();
        }
    });
}
