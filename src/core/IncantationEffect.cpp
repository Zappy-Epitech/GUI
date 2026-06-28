/**
 * @file IncantationEffect.cpp
 * @ingroup gui_core
 * @brief Implements incantation visuals: start/finish helpers, 3D drawing and update systems.
 */
#include "IncantationEffect.hpp"
#include "src/core/Core.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/gameplay/Grid.hpp"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Simulation.hpp"
#include "src/gameplay/WorldLookup.hpp"
#include "src/scenes/Game.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <format>
#include <raylib.h>
#include <raymath.h>

namespace {

constexpr float resultDuration = 1.2f;
constexpr float minEffectDuration = 1.2f;
constexpr float zappyIncantationTicks = 300.0f;

/// Returns a stable pseudo-random value in [0, 1].
static float hash01(int value) {
    value = (value << 13) ^ value;
    const int hashed = (value * (value * value * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - static_cast<float>(hashed) / 1073741824.0f * 0.5f;
}

/// Returns the visible duration for an incantation based on server frequency.
static float incantationDuration(const flecs::world &world) {
    const SimulationTime *time = world.try_get<SimulationTime>();
    const int frequency = std::max(time != nullptr ? time->timeUnit : 100, 1);

    return std::max(zappyIncantationTicks / static_cast<float>(frequency), minEffectDuration);
}

/// Finds the active incantation visual for a tile.
static flecs::entity findIncantationAt(const flecs::world &world, int x, int y) {
    return world.query<IncantationEffect>()
        .find([x, y](IncantationEffect &effect) {
            return effect.x == x && effect.y == y && !effect.finished;
        });
}

/// Removes participant links that point to an incantation.
static void clearParticipants(const flecs::world &world, flecs::entity_t incantation) {
    world.query<IncantationParticipant>()
        .each([incantation](flecs::entity player, IncantationParticipant &participant) {
            if (participant.incantation == incantation) {
                player.remove<IncantationParticipant>();
                player.remove<Incantating>();
            }
        });
}

/// Draws a blocky ring on the tile floor.
static void drawTileRing(Vector3 center, float radius, float pulse, Color color) {
    constexpr int segments = 8;
    const float cubeSize = 0.11f + pulse * 0.035f;

    for (int i = 0; i < segments; i++) {
        const float angle = static_cast<float>(i) / static_cast<float>(segments) * PI * 2.0f;
        Vector3 pos = {
            center.x + std::cos(angle) * radius,
            center.y,
            center.z + std::sin(angle) * radius,
        };

        DrawCube(pos, cubeSize, 0.055f, cubeSize, color);
    }
}

/// Draws rising cube particles around an incantation.
static void drawIncantationParticles(Vector3 center, const IncantationEffect &effect, Color color) {
    constexpr int particleCount = 18;
    const float progress = effect.finished ? 1.0f : std::clamp(effect.elapsed / effect.duration, 0.0f, 1.0f);

    for (int i = 0; i < particleCount; i++) {
        const float phase = std::fmod(progress * 2.0f + hash01(i * 17 + effect.x * 31 + effect.y * 47), 1.0f);
        const float angle = hash01(i * 91 + 3) * PI * 2.0f + effect.elapsed * (0.65f + hash01(i * 13));
        const float radius = 0.35f + hash01(i * 23 + 11) * 0.55f;
        const float size = 0.045f + hash01(i * 29 + 19) * 0.035f;
        Vector3 pos = {
            center.x + std::cos(angle) * radius,
            center.y + 0.12f + phase * 1.65f,
            center.z + std::sin(angle) * radius,
        };

        DrawCube(pos, size, size, size, Fade(color, 0.35f + phase * 0.55f));
    }
}

/// Draws final success/failure burst particles.
static void drawResultBurst(Vector3 center, const IncantationEffect &effect) {
    const float t = std::clamp(1.0f - effect.resultLifetime / resultDuration, 0.0f, 1.0f);
    const Color color = effect.success ? GOLD : RED;
    constexpr int particleCount = 16;

    for (int i = 0; i < particleCount; i++) {
        const float angle = static_cast<float>(i) / static_cast<float>(particleCount) * PI * 2.0f;
        const float radius = effect.success ? 0.25f + t * 1.05f : 0.9f - t * 0.35f;
        const float y = center.y + (effect.success ? 0.25f + t * 1.15f : 1.2f - t * 0.75f);
        Vector3 pos = {
            center.x + std::cos(angle) * radius,
            y,
            center.z + std::sin(angle) * radius,
        };

        DrawCube(pos, 0.08f, 0.08f, 0.08f, Fade(color, 1.0f - t * 0.45f));
    }
}

/// Draws one incantation tile effect.
static void drawIncantationEffect(const IncantationEffect &effect) {
    Position3 tilePosition = Grid::position(effect.x, effect.y).with_y(1.02f);
    Vector3 center = std::bit_cast<Vector3>(tilePosition);
    const float progress = effect.finished ? 1.0f : std::clamp(effect.elapsed / effect.duration, 0.0f, 1.0f);
    const float pulse = 0.5f + std::sin(effect.elapsed * 7.0f) * 0.5f;
    const Color activeColor = Color{ 125, 210, 255, 255 };
    const Color resultColor = effect.success ? Color{ 130, 255, 95, 255 } : Color{ 255, 70, 55, 255 };
    const Color color = effect.finished ? resultColor : activeColor;

    drawTileRing(center, 0.55f + pulse * 0.08f, pulse, Fade(color, 0.82f));
    drawTileRing(center, 0.32f + progress * 0.12f, 1.0f - pulse, Fade(WHITE, 0.55f));
    DrawCylinderWiresEx(
        Vector3{ center.x, center.y + 0.05f, center.z },
        Vector3{ center.x, center.y + 1.45f, center.z },
        0.18f + pulse * 0.04f,
        0.38f + pulse * 0.06f,
        4,
        Fade(color, effect.finished ? 0.45f : 0.65f));
    drawIncantationParticles(center, effect, color);

    if (effect.finished) {
        drawResultBurst(center, effect);
    }
}

/// Draws a readable blocky player participant marker.
static void drawParticipantMarker(const Position3 &position) {
    Vector3 center = std::bit_cast<Vector3>(position);
    center.y += 0.045f;
    const float pulse = 0.5f + std::sin(GetTime() * 8.0f) * 0.5f;

    drawTileRing(center, 0.27f + pulse * 0.03f, pulse, Fade(Color{ 145, 225, 255, 255 }, 0.78f));
    DrawCube(Vector3{ center.x, center.y + 0.92f + pulse * 0.08f, center.z }, 0.075f, 0.075f, 0.075f, Fade(WHITE, 0.8f));
}

} // namespace

/// Starts an incantation visual effect and marks participating players.
flecs::entity startIncantationEffect(
    const flecs::world &world,
    int x,
    int y,
    int level,
    const std::vector<int> &playerIds) {
    flecs::entity effect = findIncantationAt(world, x, y);

    if (!effect) {
        effect = world.entity(std::format("Incantation({}, {})", x, y).c_str())
            .set(IncantationEffect{
                .x = x,
                .y = y,
                .level = level,
                .elapsed = 0.0f,
                .duration = incantationDuration(world),
                .finished = false,
                .success = false,
                .resultLifetime = 0.0f,
            })
            .add<DespawnOnExit>(sceneId<Game>(world));
    } else {
        effect.set(IncantationEffect{
            .x = x,
            .y = y,
            .level = level,
            .elapsed = 0.0f,
            .duration = incantationDuration(world),
            .finished = false,
            .success = false,
            .resultLifetime = 0.0f,
        });
    }

    for (int id : playerIds) {
        flecs::entity player = findPlayer(world, id);

        if (player) {
            player.add<Incantating>();
            player.set(IncantationParticipant{ effect.id() });
        }
    }

    return effect;
}

/// Finishes an incantation visual effect and switches it to success/failure feedback.
void finishIncantationEffect(const flecs::world &world, int x, int y, bool success) {
    flecs::entity effectEntity = findIncantationAt(world, x, y);

    if (!effectEntity) {
        effectEntity = world.entity(std::format("IncantationResult({}, {})", x, y).c_str())
            .add<DespawnOnExit>(sceneId<Game>(world));
    }

    IncantationEffect effect = effectEntity.try_get<IncantationEffect>() != nullptr
        ? *effectEntity.try_get<IncantationEffect>()
        : IncantationEffect{ .x = x, .y = y };

    effect.x = x;
    effect.y = y;
    effect.finished = true;
    effect.success = success;
    effect.resultLifetime = resultDuration;
    effectEntity.set(effect);
}

/// Registers incantation visual components and systems.
IncantationEffects::IncantationEffects(flecs::world &world) {
    flecs::entity module = world.module<IncantationEffects>("incantation_effects").child_of<Core>();
    (void)module;

    world.import<Spatial>();
    world.import<Raylib>();

    world.component<IncantationEffect>();
    world.component<IncantationParticipant>();

    world.system<IncantationEffect>("UpdateIncantationEffects")
        .kind(flecs::OnUpdate)
        .each([](flecs::entity entity, IncantationEffect &effect) {
            const float dt = GetFrameTime();

            effect.elapsed += dt;
            if (effect.finished) {
                effect.resultLifetime -= dt;
                if (effect.resultLifetime <= 0.0f) {
                    clearParticipants(entity.world(), entity.id());
                    entity.destruct();
                }
            }
        });

    world.system<const IncantationEffect>("DrawIncantationEffects")
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            const flecs::world renderWorld = it.world();

            while (it.next()) {
                auto effects = it.field<const IncantationEffect>(0);

                for (auto i : it) {
                    const Position3 tilePosition = Grid::position(effects[i].x, effects[i].y);

                    if (!isWithinRenderDistance(renderWorld, std::bit_cast<Vector3>(tilePosition))) {
                        continue;
                    }
                    drawIncantationEffect(effects[i]);
                }
            }
        });

    world.system<const Position3, const IncantationParticipant>("DrawIncantationParticipantMarkers")
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            const flecs::world renderWorld = it.world();

            while (it.next()) {
                auto positions = it.field<const Position3>(0);

                for (auto i : it) {
                    if (!isWithinRenderDistance(renderWorld, std::bit_cast<Vector3>(positions[i]))) {
                        continue;
                    }
                    drawParticipantMarker(positions[i]);
                }
            }
        });
}
