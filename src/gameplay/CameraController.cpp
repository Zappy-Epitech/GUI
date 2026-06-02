#include "CameraController.hpp"
#include "src/core/Gui.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include <cmath>
#include <raylib.h>

namespace {

/// The grid center position for the camera.
static constexpr Vector3 gridCenter = { 4.95f, 0.0f, 4.95f };

/// The controlled camera instance.
static Camera controlledCamera = {
    Vector3{ gridCenter.x, 8.0f, -10.0f },
    gridCenter,
    Vector3{ 0.0f, 1.0f, 0.0f },
    45.0f,
    CAMERA_PERSPECTIVE,
};

/// The camera yaw angle.
static float cameraYaw = 0.0f;

/// The camera pitch angle.
static float cameraPitch = -0.491532f;

/// Checks forward movement input.
static bool isForwardDown() {
    return IsKeyDown(KEY_W);
}

/// Checks left movement input.
static bool isLeftDown() {
    return IsKeyDown(KEY_A);
}
} // namespace

static constexpr float moveSpeed = 18.0f;
static constexpr float lookSpeed = 1.8f;
static constexpr float minPitch = -1.35f;
static constexpr float maxPitch = 1.35f;

/// Registers camera control systems.
CameraController::CameraController(flecs::world &world) {
    world.module<CameraController>("cameraController").child_of<GamePlay>();

    world.system("CameraControls")
        .read<HasInputActive>()
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &it) {
            if (it.world().get<HasInputActive>().value) {
                return;
            }

            float moveStep = moveSpeed * it.delta_time();
            float lookStep = lookSpeed * it.delta_time();

            if (IsKeyDown(KEY_RIGHT)) {
                cameraYaw -= lookStep;
            }
            if (IsKeyDown(KEY_LEFT)) {
                cameraYaw += lookStep;
            }
            if (IsKeyDown(KEY_UP)) {
                cameraPitch += lookStep;
            }
            if (IsKeyDown(KEY_DOWN)) {
                cameraPitch -= lookStep;
            }
            if (cameraPitch < minPitch) {
                cameraPitch = minPitch;
            }
            if (cameraPitch > maxPitch) {
                cameraPitch = maxPitch;
            }

            Vector3 forward = {
                std::sin(cameraYaw),
                0.0f,
                std::cos(cameraYaw),
            };
            Vector3 right = {
                std::cos(cameraYaw),
                0.0f,
                -std::sin(cameraYaw),
            };

            if (isForwardDown()) {
                controlledCamera.position.x += forward.x * moveStep;
                controlledCamera.position.z += forward.z * moveStep;
            }
            if (IsKeyDown(KEY_S)) {
                controlledCamera.position.x -= forward.x * moveStep;
                controlledCamera.position.z -= forward.z * moveStep;
            }
            if (isLeftDown()) {
                controlledCamera.position.x += right.x * moveStep;
                controlledCamera.position.z += right.z * moveStep;
            }
            if (IsKeyDown(KEY_D)) {
                controlledCamera.position.x -= right.x * moveStep;
                controlledCamera.position.z -= right.z * moveStep;
            }
            if (IsKeyDown(KEY_Q)) {
                controlledCamera.position.y -= moveStep;
            }
            if (IsKeyDown(KEY_E)) {
                controlledCamera.position.y += moveStep;
            }

            Vector3 lookDirection = {
                std::cos(cameraPitch) * std::sin(cameraYaw),
                std::sin(cameraPitch),
                std::cos(cameraPitch) * std::cos(cameraYaw),
            };

            controlledCamera.target = {
                controlledCamera.position.x + lookDirection.x,
                controlledCamera.position.y + lookDirection.y,
                controlledCamera.position.z + lookDirection.z,
            };
        });
}

/// Returns the shared camera.
Camera &CameraController::camera() {
    return controlledCamera;
}
