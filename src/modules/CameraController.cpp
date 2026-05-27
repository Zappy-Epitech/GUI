#include "CameraController.hpp"
#include "../extern/flecs.hpp"
#include "src/modules/Gui.hpp"
#include <cmath>
#include <raylib.h>

namespace {
static constexpr Vector3 gridCenter = { 4.95f, 0.0f, 4.95f };

static Camera controlledCamera = {
    Vector3{ gridCenter.x, 8.0f, -10.0f },
    gridCenter,
    Vector3{ 0.0f, 1.0f, 0.0f },
    45.0f,
    CAMERA_PERSPECTIVE,
};

static float cameraYaw = 0.0f;
static float cameraPitch = -0.491532f;

static bool isForwardDown() {
    return IsKeyDown(KEY_W);
}

static bool isLeftDown() {
    return IsKeyDown(KEY_A);
}
} // namespace

static constexpr float moveSpeed = 18.0f;
static constexpr float lookSpeed = 1.8f;
static constexpr float minPitch = -1.35f;
static constexpr float maxPitch = 1.35f;

CameraController::CameraController(flecs::world &world) {
    world.module<CameraController>();

    world.system("Camera Controls")
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

Camera &CameraController::camera() {
    return controlledCamera;
}
