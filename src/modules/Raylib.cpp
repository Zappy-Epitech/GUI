#include "Raylib.hpp"
#include "../extern/flecs.hpp"
#include "CameraController.hpp"
#include "Spatial.hpp"
#include "src/MinecraftRenderer.hpp"
#include <bit>
#include <raygui.h>
#include <raylib.h>

Raylib::Raylib(flecs::world &world) {
    world.module<Raylib>();
    world.import<Spatial>();

    world.component<Color>();
    world.component<Cube>();
    world.component<Model>();

    world.component<Render3D>()
        .add(flecs::Phase)
        .depends_on(flecs::PostUpdate);

    world.component<Render2D>()
        .add(flecs::Phase)
        .depends_on<Render3D>();

    world.component<PostRender>()
        .add(flecs::Phase)
        .depends_on<Render2D>();

    world.system("Setup Window")
        .kind(flecs::OnStart)
        .run([](flecs::iter &) {
            SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
            SetConfigFlags(FLAG_MSAA_4X_HINT);
            InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Zappy");
            SetTargetFPS(120);
            InitAudioDevice();
            GuiLoadStyleDefault();
            GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 5);
        });

    world.system("BeginFrame").kind(flecs::PreUpdate).run([](flecs::iter &it) {
        if (WindowShouldClose()) {
            return it.world().quit();
        }
        BeginDrawing();
        ClearBackground(BLACK);
    });

    world.system("Begin3D")
        .kind<Render3D>()
        .run([](flecs::iter &) {
            BeginMode3D(CameraController::camera());
        });

    world.system<Position3, Size2, Color>("Render Cube")
        .kind<Render3D>()
        .with<Cube>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<Position3>(0);
                auto sizes = it.field<Size2>(1);
                auto colors = it.field<Color>(2);

                for (auto i : it) {
                    DrawCube(std::bit_cast<Vector3>(positions[i]), sizes[i].width, sizes[i].height, sizes[i].height, colors[i]);
                    DrawCubeWires(std::bit_cast<Vector3>(positions[i]), sizes[i].width, sizes[i].height, sizes[i].height, RED);
                }
            }
        });

    // static Texture2D texture;

    // world.system().kind(flecs::OnStart).run([](flecs::iter &) {
    //     texture = LoadTexture("./assets/ping.png");
    //     SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    // });

    // world.system().kind<Render3D>().run([](flecs::iter &) {
    //     DrawMinecraftPlayer(texture, Vector3(0, 0, 0), 8, 0.0f);
    // });

    world.system<const Position3, const Model, const Scale>("Render Model")
        .kind<Render3D>()
        .each([](const Position3 &position, const Model &model, const Scale &scale) {
            DrawModel(model, std::bit_cast<Vector3>(position), scale.value, WHITE);
        });

    world.system("End3D")
        .kind<Render3D>()
        .run([](flecs::iter &) {
            EndMode3D();
        });

    world.system("EndFrame")
        .kind<PostRender>()
        .run([](flecs::iter &) {
            if (IsWindowReady()) {
                EndDrawing();
            }
        });
}
