#include "Movement.hpp"
#include "../extern/flecs.h"
#include "src/core/Spatial.hpp"
#include "src/gameplay/GamePlay.hpp"
#include "src/gameplay/Simulation.hpp"
#include <algorithm>
#include <cmath>

namespace {

constexpr int minFrequency = 1;
constexpr int maxFrequency = 1000;
constexpr float movementActionTimeUnits = 7.0f;
constexpr float minMovementDuration = 0.08f;
constexpr float maxMovementDuration = 1.2f;

} // namespace

/// Returns the player interpolation speed for a movement distance and server frequency.
float movementSpeedForFrequency(float distance, int frequency) {
    const int clampedFrequency = std::clamp(frequency, minFrequency, maxFrequency);
    const float duration = std::clamp(movementActionTimeUnits / static_cast<float>(clampedFrequency), minMovementDuration, maxMovementDuration);

    return distance / duration;
}

/// Registers movement interpolation systems.
Movement::Movement(flecs::world &world) {
    world.module<Movement>("movement").child_of<GamePlay>();

    world.component<Direction>()
        .member<float>("x")
        .member<float>("y")
        .member<float>("speed");

    // direction.y represent the direction along the z-axis
    world.system<Position3, const Direction>("Movement")
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<Position3>(0);
                auto directions = it.field<const Direction>(1);
                const float dt = it.delta_time();

                for (auto i : it) {
                    auto &pos = positions[i];
                    const auto &dir = directions[i];

                    const float dx = dir.x - pos.x;
                    const float dy = dir.y - pos.z;

                    const float dist = std::sqrt(dx * dx + dy * dy);

                    if (dist <= 0.01f) {
                        pos.x = dir.x;
                        pos.z = dir.y;

                        it.entity(i).remove<Direction>();
                        continue;
                    }

                    const float move = std::min(dist, std::max(dir.speed, 0.01f) * dt);

                    pos.x += (dx / dist) * move;
                    pos.z += (dy / dist) * move;
                }
            }
        });
}
