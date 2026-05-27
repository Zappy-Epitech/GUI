#include "Raylib.hpp"
#include "../extern/flecs.hpp"
#include "CameraController.hpp"
#include "Spatial.hpp"
#include <bit>
#include <raygui.h>
#include <raylib.h>

Raylib::Raylib(flecs::world &world) {
    world.module<Raylib>();
    world.import<Spatial>();

    world.component<Color>();
    world.component<Cube>();

    world.system("Setup Window").kind(flecs::OnStart).run([](flecs::iter &) {
        SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Zappy");
        SetTargetFPS(120);
        GuiLoadStyleDefault();
        GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
        GuiSetStyle(DEFAULT, TEXT_SPACING, 5);
    });

    world.system("Begin Frame").kind(flecs::PreUpdate).run([](flecs::iter &it) {
        if (WindowShouldClose()) {
            return it.world().quit();
        }
        BeginDrawing();
        ClearBackground(BLACK);
    });

    world.system<Position3, Size2, Color>("Render Cube")
        .kind(flecs::OnUpdate)
        .with<Cube>()
        .run([](flecs::iter &it) {
            BeginMode3D(CameraController::camera());
            while (it.next()) {
                auto positions = it.field<Position3>(0);
                auto sizes = it.field<Size2>(1);
                auto colors = it.field<Color>(2);

                for (auto i : it) {
                    DrawCube(std::bit_cast<Vector3>(positions[i]), sizes[i].width, sizes[i].height, sizes[i].height, colors[i]);
                    DrawCubeWires(std::bit_cast<Vector3>(positions[i]), sizes[i].width, sizes[i].height, sizes[i].height, RED);
                }
            }
            EndMode3D();
        });

    world.system("End Frame").kind(flecs::PostUpdate).run([](flecs::iter &) {
        if (IsWindowReady()) {
            EndDrawing();
        }
    });
}
