#include "Movement.hpp"
#include "../extern/flecs.h"
#include "src/core/Spatial.hpp"
#include "src/gameplay/GamePlay.hpp"
#include <algorithm>
#include <cmath>

Movement::Movement(flecs::world &world) {
    world.module<Movement>("movement").child_of<GamePlay>();

    world.component<Direction>()
        .member<float>("x")
        .member<float>("y");

    // direction.y represent the direction along the z-axis
    world.system<Position3, const Direction>()
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

                    const float speedFactor = 1.0f;
                    const float speed = dist * speedFactor;
                    const float move = std::min(dist, speed * dt);

                    pos.x += (dx / dist) * move;
                    pos.z += (dy / dist) * move;
                }
            }
        });
}
