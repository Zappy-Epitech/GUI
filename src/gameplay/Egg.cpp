#include "Egg.hpp"
#include "src/core/Core.hpp"
#include "src/core/Raylib.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/minecraft/MinecraftAnimation.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>

namespace {

constexpr float eggModelScale = 0.12f;
constexpr float eggRiseHeight = 0.18f;
constexpr int dustParticleCount = 8;
constexpr float forkActionTimeUnits = 42.0f;
constexpr float defaultServerFrequency = 100.0f;

/// Draws the square dust ring emitted while an egg pops out of the ground.
static void drawDustRing(Position3 center, float t, float maxRadius, float maxSize) {
    const float radius = 0.12f + t * 0.28f;
    const float size = (1.0f - t) * maxSize;

    if (size <= 0.0f) {
        return;
    }

    for (int i = 0; i < dustParticleCount; i++) {
        const float angle = static_cast<float>(i) * (2.0f * PI / static_cast<float>(dustParticleCount));
        Vector3 particle = {
            center.x + std::cos(angle) * std::min(radius, maxRadius),
            center.y,
            center.z + std::sin(angle) * std::min(radius, maxRadius),
        };

        DrawCube(particle, size, size, size, Fade(BROWN, 1.0f - t));
    }
}

/// Draws the square dust ring emitted while an egg pops out of the ground.
static void drawEggDust(const EggSpawnAnimation &animation, float t) {
    drawDustRing(animation.basePosition.add_y(-0.78f + t * 0.04f), t, 0.4f, 0.045f);
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

/// Computes and applies the player laying pose.
static void updateLayingPose(EggLayingAnimation &animation, Position3 &position, SkinPose &pose) {
    const float t = std::clamp(animation.elapsed / animation.duration, 0.0f, 1.0f);
    const float settle = 1.0f - std::pow(1.0f - std::min(t * 3.0f, 1.0f), 3.0f);
    const float release = t > 0.82f ? (t - 0.82f) / 0.18f : 0.0f;
    const float poseAmount = settle * (1.0f - release);

    position = animation.basePosition;
    pose.bodyPitch = 22.0f * poseAmount;
    pose.angles[static_cast<int>(Limb::LeftLeg)] = 0.0f;
    pose.angles[static_cast<int>(Limb::RightLeg)] = 0.0f;
    pose.angles[static_cast<int>(Limb::LeftArm)] = -28.0f * poseAmount;
    pose.angles[static_cast<int>(Limb::RightArm)] = -28.0f * poseAmount;
}

/// Returns the current server frequency used to convert seconds to game time units.
static float serverFrequency(const flecs::world &world) {
    const SimulationTime *time = world.try_get<SimulationTime>();

    return std::max(time != nullptr ? static_cast<float>(time->timeUnit) : defaultServerFrequency, 1.0f);
}

} // namespace

/// Starts the visible player laying sequence.
void startEggLayingAnimation(const flecs::world &world, int playerId) {
    flecs::entity player = findPlayer(world, playerId);

    if (!player) {
        return;
    }

    const Position3 *position = player.try_get<Position3>();
    if (position == nullptr) {
        return;
    }

    player.set(EggLayingAnimation{ *position, 0.0f, forkActionTimeUnits });
}

/// Registers egg gameplay visuals.
Eggs::Eggs(flecs::world &world) {
    flecs::entity module = world.module<Eggs>("eggs").child_of<GamePlay>();
    (void)module;

    world.import<Raylib>();
    world.component<EggId>();
    world.component<EggSpawnAnimation>();
    world.component<EggLayingAnimation>();

    world.system<EggLayingAnimation, Position3, SkinPose>("UpdateEggLayingAnimations")
        .kind(flecs::PostUpdate)
        .each([](flecs::entity entity, EggLayingAnimation &animation, Position3 &position, SkinPose &pose) {
            animation.elapsed += GetFrameTime() * serverFrequency(entity.world());
            updateLayingPose(animation, position, pose);
            if (animation.elapsed >= animation.duration) {
                position = animation.basePosition;
                entity.remove<EggLayingAnimation>();
            }
        });

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
