#include "Raylib.hpp"
#include "../extern/flecs.hpp"
#include <cstdio>
#include <raylib.h>

Raylib::Raylib(flecs::world &world) {
    world.module<Raylib>();

    world.system("Setup Window").kind(flecs::OnStart).run([](flecs::iter &) {
        InitWindow(1920, 1080, "Zappy");
        SetTargetFPS(0);
    });

    world.system("Begin Frame").kind(flecs::PreUpdate).run([](flecs::iter &it) {
        if (WindowShouldClose()) {
            return it.world().quit();
        }
        if (IsWindowMinimized() || !IsWindowFocused()) {
            return;
        }
        BeginDrawing();
        ClearBackground(BLACK);
    });

    world.system("End Frame").kind(flecs::PostUpdate).run([](flecs::iter &it) {
        if (IsWindowReady() && !IsWindowMinimized() && IsWindowFocused()) {
            EndDrawing();
        }
    });
}
