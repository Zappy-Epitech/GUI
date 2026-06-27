#include "Egg.hpp"
#include "src/core/Core.hpp"
#include "src/core/Raylib.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>

namespace {

constexpr float eggModelScale = 0.12f;
constexpr float eggRiseHeight = 0.18f;
constexpr int dustParticleCount = 8;

/// Draws the square dust ring emitted while an egg pops out of the ground.
static void drawEggDust(const EggSpawnAnimation &animation, float t) {
    const float radius = 0.12f + t * 0.28f;
    const float size = (1.0f - t) * 0.045f;

    if (size <= 0.0f) {
        return;
    }

    for (int i = 0; i < dustParticleCount; i++) {
        const float angle = static_cast<float>(i) * (2.0f * PI / static_cast<float>(dustParticleCount));
        Vector3 particle = {
            animation.basePosition.x + std::cos(angle) * radius,
            animation.basePosition.y - 0.78f + t * 0.04f,
            animation.basePosition.z + std::sin(angle) * radius,
        };

        DrawCube(particle, size, size, size, Fade(BROWN, 1.0f - t));
    }
}

/// Computes and applies the pop transform for an animated egg.
static void updateEggPop(EggSpawnAnimation &animation, Position3 &position, Scale &scale) {
    const float t = std::clamp(animation.elapsed / animation.duration, 0.0f, 1.0f);
    const float pop = 1.0f - std::pow(1.0f - t, 3.0f);
    const float bounce = std::sin(t * PI * 2.0f) * (1.0f - t) * 0.045f;
    const float overshoot = std::sin(t * PI) * 0.18f;

    position = animation.basePosition.add_y(-eggRiseHeight + pop * eggRiseHeight + bounce);
    scale.value = eggModelScale * std::min(pop + overshoot, 1.12f);
}

} // namespace

/// Registers egg gameplay visuals.
Eggs::Eggs(flecs::world &world) {
    flecs::entity module = world.module<Eggs>("eggs").child_of<GamePlay>();
    (void)module;

    world.import<Raylib>();
    world.component<EggId>();
    world.component<EggSpawnAnimation>();

    world.system<EggSpawnAnimation, Position3, Scale>("UpdateEggSpawnAnimations")
        .kind(flecs::OnUpdate)
        .each([](flecs::entity entity, EggSpawnAnimation &animation, Position3 &position, Scale &scale) {
            animation.elapsed += GetFrameTime();
            updateEggPop(animation, position, scale);
            if (animation.elapsed >= animation.duration) {
                position = animation.basePosition;
                scale.value = eggModelScale;
                entity.remove<EggSpawnAnimation>();
            }
        });

    world.system<const EggSpawnAnimation>("DrawEggSpawnDust")
        .kind<Draw3D>()
        .each([](const EggSpawnAnimation &animation) {
            const float t = std::clamp(animation.elapsed / animation.duration, 0.0f, 1.0f);
            drawEggDust(animation, t);
        });
}
