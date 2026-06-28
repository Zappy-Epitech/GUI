#include "Raylib.hpp"
#include "Settings.hpp"
#include "Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/CameraController.hpp"
#include "src/scenes/Game.hpp"
#include "src/scenes/Home.hpp"
#include <bit>
#include <raygui.h>
#include <raylib.h>
#include <raymath.h>
#include <type_traits>

static_assert(sizeof(Position3) == sizeof(Vector3));
static_assert(std::is_trivially_copyable_v<Position3>);
static_assert(std::is_trivially_copyable_v<Vector3>);

/// Returns whether a world position is close enough to be rendered.
bool isWithinRenderDistance(const flecs::world &world, Vector3 position) {
    const GuiSettings *settings = world.try_get<GuiSettings>();

    if (settings == nullptr || settings->renderDistance <= 0.0f) {
        return true;
    }

    const Vector3 cameraPosition = CameraController::camera().position;
    const float dx = position.x - cameraPosition.x;
    const float dz = position.z - cameraPosition.z;
    const float maxDistance = settings->renderDistance;

    return dx * dx + dz * dz <= maxDistance * maxDistance;
}

/// Registers Raylib phases and render systems.
Raylib::Raylib(flecs::world &world) {
    world.module<Raylib>("raylib");
    world.import<Spatial>();

    world.component<Color>();
    world.component<Cube>();
    world.component<Model>();
    world.component<Texture2D>();

    world.component<Render3D>()
        .add(flecs::Phase)
        .depends_on(flecs::PostUpdate);

    world.component<Draw3D>()
        .add(flecs::Phase)
        .depends_on<Render3D>();

    world.component<CloseRender3D>()
        .add(flecs::Phase)
        .depends_on<Draw3D>();

    world.component<Render2D>()
        .add(flecs::Phase)
        .depends_on<CloseRender3D>();

    world.component<PostRender>()
        .add(flecs::Phase)
        .depends_on<Render2D>();

    world.system("SetupWindow")
        .kind(flecs::OnStart)
        .run([world](flecs::iter &) {
            SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
            SetConfigFlags(FLAG_MSAA_4X_HINT);
            InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Zappy");
            SetTargetFPS(240);
            InitAudioDevice();
            GuiLoadStyleDefault();
            GuiSetStyle(DEFAULT, TEXT_SIZE, 32);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 5);
            GuiSetIconScale(2);
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

    world.system<Position3, Size2, Color>("RenderCube")
        .kind<Draw3D>()
        .with<Cube>()
        .run([](flecs::iter &it) {
            const flecs::world renderWorld = it.world();

            while (it.next()) {
                auto transforms = it.field<Position3>(0);
                auto sizes = it.field<Size2>(1);
                auto colors = it.field<Color>(2);

                for (auto i : it) {
                    const Vector3 position = std::bit_cast<Vector3>(transforms[i]);

                    if (!isWithinRenderDistance(renderWorld, position)) {
                        continue;
                    }
                    DrawCube(position, sizes[i].width, sizes[i].height, sizes[i].width, colors[i]);
                    DrawCubeWires(position, sizes[i].width, sizes[i].height, sizes[i].width, RED);
                }
            }
        });

    world.system<const Position3, const Model, const Scale>("RenderModel")
        .kind<Draw3D>()
        .without<Rotation3>()
        .run([](flecs::iter &it) {
            const flecs::world renderWorld = it.world();

            while (it.next()) {
                auto transforms = it.field<const Position3>(0);
                auto models = it.field<const Model>(1);
                auto scales = it.field<const Scale>(2);

                for (auto i : it) {
                    const Vector3 position = std::bit_cast<Vector3>(transforms[i]);

                    if (!isWithinRenderDistance(renderWorld, position)) {
                        continue;
                    }
                    DrawModel(models[i], position, scales[i].value, WHITE);
                }
            }
        });

    world.system<const Position3, const Model, const Scale, const Rotation3>("RenderRotatedModel")
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            const flecs::world renderWorld = it.world();

            while (it.next()) {
                auto transforms = it.field<const Position3>(0);
                auto models = it.field<const Model>(1);
                auto scales = it.field<const Scale>(2);
                auto rotations = it.field<const Rotation3>(3);

                for (auto i : it) {
                    const Vector3 position = std::bit_cast<Vector3>(transforms[i]);

                    if (!isWithinRenderDistance(renderWorld, position)) {
                        continue;
                    }

                    Model rotatedModel = models[i];
                    Matrix rotationMatrix = MatrixRotateZYX(Vector3{
                        rotations[i].x * DEG2RAD,
                        rotations[i].y * DEG2RAD,
                        rotations[i].z * DEG2RAD,
                    });
                    rotatedModel.transform = MatrixMultiply(rotatedModel.transform, rotationMatrix);

                    DrawModel(rotatedModel, position, scales[i].value, WHITE);
                }
            }
        });

    world.system("End3D")
        .kind<CloseRender3D>()
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
