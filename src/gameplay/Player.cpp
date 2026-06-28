/**
 * @file Player.cpp
 * @ingroup gui_gameplay
 * @brief Implements player expulsion animation and its flecs system.
 */
#include "Player.hpp"
#include "src/core/Raylib.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/GamePlay.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/minecraft/MinecraftAnimation.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>

namespace {

constexpr float expelActionTimeUnits = 7.0f;
constexpr float defaultServerFrequency = 100.0f;

/// Returns the current server frequency used to convert seconds to game time units.
static float serverFrequency(const flecs::world &world) {
    const SimulationTime *time = world.try_get<SimulationTime>();

    return std::max(time != nullptr ? static_cast<float>(time->timeUnit) : defaultServerFrequency, 1.0f);
}

/// Computes and applies the arm-only expulsion pose.
static void updateExpelPose(PlayerExpelAnimation &animation, SkinPose &pose) {
    const float t = std::clamp(animation.elapsed / animation.duration, 0.0f, 1.0f);
    const float rise = 1.0f - std::pow(1.0f - std::min(t * 4.0f, 1.0f), 3.0f);
    const float release = t > 0.55f ? (t - 0.55f) / 0.45f : 0.0f;
    const float amount = rise * (1.0f - release);

    pose.bodyPitch = 0.0f;
    pose.angles[static_cast<int>(Limb::LeftLeg)] = 0.0f;
    pose.angles[static_cast<int>(Limb::RightLeg)] = 0.0f;
    pose.angles[static_cast<int>(Limb::LeftArm)] = -72.0f * amount;
    pose.angles[static_cast<int>(Limb::RightArm)] = -72.0f * amount;
}

} // namespace

/// Starts the visible player expulsion sequence.
void startPlayerExpelAnimation(const flecs::world &world, int playerId) {
    flecs::entity player = findPlayer(world, playerId);

    if (!player) {
        return;
    }

    player.set(PlayerExpelAnimation{ 0.0f, expelActionTimeUnits });
}

/// Registers player gameplay visuals.
Players::Players(flecs::world &world) {
    flecs::entity module = world.module<Players>("players").child_of<GamePlay>();
    (void)module;

    world.import<Raylib>();
    world.component<Player>();
    world.component<PlayerId>();
    world.component<PlayerSkin>();
    world.component<Incantating>();
    world.component<PlayerBroadcastBubble>();
    world.component<PlayerExpelAnimation>();

    world.system<PlayerExpelAnimation, SkinPose>("UpdatePlayerExpelAnimations")
        .kind(flecs::PostUpdate)
        .each([](flecs::entity entity, PlayerExpelAnimation &animation, SkinPose &pose) {
            animation.elapsed += GetFrameTime() * serverFrequency(entity.world());
            updateExpelPose(animation, pose);
            if (animation.elapsed >= animation.duration) {
                entity.remove<PlayerExpelAnimation>();
            }
        });
}
