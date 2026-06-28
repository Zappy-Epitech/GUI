#include "MinecraftRenderer.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Settings.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Player.hpp"
#include "src/gameplay/Team.hpp"
#include "src/minecraft/MinecraftAnimation.hpp"
#include "src/scenes/GameUi.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <map>
#include <raylib.h>
#include <rlgl.h>
#include <utility>
#include <vector>

/// Stores one face UV rectangle.
struct FaceUV {
    float x, y, w, h;
};
/// Stores UVs for a body part.
struct PartUV {
    FaceUV right, front, left, back, top, bottom;
};

/// The UVs for all part of minecraft skin.
static constexpr PartUV HEAD = { { 0, 8, 8, 8 }, { 8, 8, 8, 8 }, { 16, 8, 8, 8 }, { 24, 8, 8, 8 }, { 8, 0, 8, 8 }, { 16, 0, 8, 8 } };
static constexpr PartUV BODY = { { 16, 20, 4, 12 }, { 20, 20, 8, 12 }, { 28, 20, 4, 12 }, { 32, 20, 8, 12 }, { 20, 16, 8, 4 }, { 28, 16, 8, 4 } };
static constexpr PartUV RARM = { { 40, 20, 4, 12 }, { 44, 20, 4, 12 }, { 48, 20, 4, 12 }, { 52, 20, 4, 12 }, { 44, 16, 4, 4 }, { 48, 16, 4, 4 } };
static constexpr PartUV LARM = { { 32, 52, 4, 12 }, { 36, 52, 4, 12 }, { 40, 52, 4, 12 }, { 44, 52, 4, 12 }, { 36, 48, 4, 4 }, { 40, 48, 4, 4 } };
static constexpr PartUV RLEG = { { 0, 20, 4, 12 }, { 4, 20, 4, 12 }, { 8, 20, 4, 12 }, { 12, 20, 4, 12 }, { 4, 16, 4, 4 }, { 8, 16, 4, 4 } };
static constexpr PartUV LLEG = { { 16, 52, 4, 12 }, { 20, 52, 4, 12 }, { 24, 52, 4, 12 }, { 28, 52, 4, 12 }, { 20, 48, 4, 4 }, { 24, 48, 4, 4 } };
static constexpr PartUV HEAD_OVERLAY = { { 32, 8, 8, 8 }, { 40, 8, 8, 8 }, { 48, 8, 8, 8 }, { 56, 8, 8, 8 }, { 40, 0, 8, 8 }, { 48, 0, 8, 8 } };
static constexpr PartUV BODY_OVERLAY = { { 16, 36, 4, 12 }, { 20, 36, 8, 12 }, { 28, 36, 4, 12 }, { 32, 36, 8, 12 }, { 20, 32, 8, 4 }, { 28, 32, 8, 4 } };
static constexpr PartUV RARM_OVERLAY = { { 40, 36, 4, 12 }, { 44, 36, 4, 12 }, { 48, 36, 4, 12 }, { 52, 36, 4, 12 }, { 44, 32, 4, 4 }, { 48, 32, 4, 4 } };
static constexpr PartUV LARM_OVERLAY = { { 48, 52, 4, 12 }, { 52, 52, 4, 12 }, { 56, 52, 4, 12 }, { 60, 52, 4, 12 }, { 52, 48, 4, 4 }, { 56, 48, 4, 4 } };
static constexpr PartUV RLEG_OVERLAY = { { 0, 36, 4, 12 }, { 4, 36, 4, 12 }, { 8, 36, 4, 12 }, { 12, 36, 4, 12 }, { 4, 32, 4, 4 }, { 8, 32, 4, 4 } };
static constexpr PartUV LLEG_OVERLAY = { { 0, 52, 4, 12 }, { 4, 52, 4, 12 }, { 8, 52, 4, 12 }, { 12, 52, 4, 12 }, { 4, 48, 4, 4 }, { 8, 48, 4, 4 } };

/// Draws a textured cuboid.
static void skinCube(Texture2D tex, const PartUV &uv, float w, float h, float d, Color tint) {
    const float tw = (float)tex.width;
    const float th = (float)tex.height;

    const Vector3 p = { w * 0.5f, h * 0.5f, d * 0.5f };

    auto quad = [&](Vector3 n, const FaceUV &f, Vector3 a, Vector3 b, Vector3 c, Vector3 d) {
        const float u0 = f.x / tw;
        const float u1 = (f.x + f.w) / tw;
        const float v0 = f.y / th;
        const float v1 = (f.y + f.h) / th;

        rlNormal3f(n.x, n.y, n.z);
        rlTexCoord2f(u0, v1);
        rlVertex3f(a.x, a.y, a.z);
        rlTexCoord2f(u1, v1);
        rlVertex3f(b.x, b.y, b.z);
        rlTexCoord2f(u1, v0);
        rlVertex3f(c.x, c.y, c.z);
        rlTexCoord2f(u0, v0);
        rlVertex3f(d.x, d.y, d.z);
    };

    rlSetTexture(tex.id);
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    quad({ 0, 0, 1 }, uv.front,
         { -p.x, -p.y, p.z }, { p.x, -p.y, p.z },
         { p.x, p.y, p.z }, { -p.x, p.y, p.z });

    quad({ 0, 0, -1 }, uv.back,
         { -p.x, -p.y, -p.z }, { -p.x, p.y, -p.z },
         { p.x, p.y, -p.z }, { p.x, -p.y, -p.z });

    quad({ 0, 1, 0 }, uv.top,
         { -p.x, p.y, -p.z }, { -p.x, p.y, p.z },
         { p.x, p.y, p.z }, { p.x, p.y, -p.z });

    quad({ 0, -1, 0 }, uv.bottom,
         { -p.x, -p.y, -p.z }, { p.x, -p.y, -p.z },
         { p.x, -p.y, p.z }, { -p.x, -p.y, p.z });

    quad({ 1, 0, 0 }, uv.right,
         { p.x, p.y, -p.z }, { p.x, p.y, p.z },
         { p.x, -p.y, p.z }, { p.x, -p.y, -p.z });

    quad({ -1, 0, 0 }, uv.left,
         { -p.x, p.y, -p.z }, { -p.x, -p.y, -p.z },
         { -p.x, -p.y, p.z }, { -p.x, p.y, p.z });

    rlEnd();
    rlColor4ub(255, 255, 255, 255);
    rlSetTexture(0);
}

/// Draws one animated limb.
static void limb(Texture2D tex, const PartUV &uv, const PartUV *overlayUv, float px, float ox, float oy, float oz, float angle, float w, float h, float d, Color tint) {
    rlPushMatrix();
    rlTranslatef(ox, oy, oz);
    rlRotatef(angle, 1, 0, 0);
    rlTranslatef(0, -h * px * .5f, 0);
    skinCube(tex, uv, w * px, h * px, d * px, tint);
    if (overlayUv != nullptr) {
        constexpr float overlayPadding = 0.045f;

        skinCube(tex, *overlayUv, (w + overlayPadding) * px, (h + overlayPadding) * px, (d + overlayPadding) * px, tint);
    }
    rlPopMatrix();
}

/// Draws a Minecraft-style player.
static void drawPlayer(Texture2D tex, Vector3 pos, float scale, float angle, const SkinPose &pose, Color tint) {
    float px = scale / 16.f;
    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);
    rlRotatef(angle, 0, 1, 0);

    if (tex.id == 0 || tex.width <= 0 || tex.height <= 0) {
        const Color body = Color{ 72, 96, 128, tint.a };
        const Color head = Color{ 210, 178, 140, tint.a };

        DrawCube(Vector3{ 0, 6 * px, 0 }, 8 * px, 12 * px, 4 * px, body);
        DrawCube(Vector3{ 0, 16 * px, 0 }, 8 * px, 8 * px, 8 * px, head);
        DrawCube(Vector3{ -6 * px, 6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
        DrawCube(Vector3{ 6 * px, 6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
        DrawCube(Vector3{ -2 * px, -6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
        DrawCube(Vector3{ 2 * px, -6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
        rlPopMatrix();
        return;
    }

    limb(tex, RLEG, &RLEG_OVERLAY, px, -2 * px, 0, 0, pose.angles[(int)Limb::RightLeg], 4, 12, 4, tint);
    limb(tex, LLEG, &LLEG_OVERLAY, px, 2 * px, 0, 0, pose.angles[(int)Limb::LeftLeg], 4, 12, 4, tint);

    rlPushMatrix();
    rlTranslatef(0, 0, 0);
    rlRotatef(pose.bodyPitch, 1, 0, 0);

    limb(tex, RARM, &RARM_OVERLAY, px, -6 * px, 12 * px, 0, pose.angles[(int)Limb::RightArm], 4, 12, 4, tint);
    limb(tex, LARM, &LARM_OVERLAY, px, 6 * px, 12 * px, 0, pose.angles[(int)Limb::LeftArm], 4, 12, 4, tint);

    rlPushMatrix();
    rlTranslatef(0, 16 * px, 0);
    skinCube(tex, HEAD, 8 * px, 8 * px, 8 * px, tint);
    skinCube(tex, HEAD_OVERLAY, 8.45f * px, 8.45f * px, 8.45f * px, tint);
    rlPopMatrix();
    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(tex, BODY, 8 * px, 12 * px, 4 * px, tint);
    skinCube(tex, BODY_OVERLAY, 8.35f * px, 12.35f * px, 4.35f * px, tint);
    rlPopMatrix();

    rlPopMatrix();

    rlPopMatrix();
}

static void drawPlayerHoverMarker(Vector3 pos, float scale) {
    const float px = scale / 16.0f;
    const float radius = scale * 0.62f;
    const float footY = pos.y - 12.0f * px;
    const float y = footY + 0.08f;

    DrawCylinderEx(
        Vector3{ pos.x, y, pos.z },
        Vector3{ pos.x, y + 0.003f, pos.z },
        radius,
        radius,
        24,
        Fade(SKYBLUE, 0.58f));
    DrawCylinderWiresEx(
        Vector3{ pos.x, y + 0.004f, pos.z },
        Vector3{ pos.x, y + 0.007f, pos.z },
        radius,
        radius,
        24,
        SKYBLUE);
}

static bool shouldHighlightPlayer(flecs::world world, flecs::entity player) {
    const GuiSettings *settings = world.try_get<GuiSettings>();
    const GameUiState *state = world.try_get<GameUiState>();

    return settings != nullptr && settings->highlightTeamOnHover && state != nullptr && state->hoveredTeam != 0 && player.has<BelongsTo>(state->hoveredTeam);
}

struct RenderPlayer {
    flecs::entity entity;
    Vector3 position;
    float yaw;
    Texture2D texture;
    float scale;
    SkinPose pose;
    int playerId;
    Vector3 offset = {};
    float crowdScale = 1.0f;
};

static std::pair<int, int> tileKey(Vector3 position) {
    constexpr float gridStep = 1.1f;

    return {
        static_cast<int>(std::round(position.x / gridStep)),
        static_cast<int>(std::round(position.z / gridStep)),
    };
}

static float crowdScaleForCount(std::size_t count) {
    if (count <= 1) {
        return 1.0f;
    }
    if (count == 2) {
        return 0.92f;
    }
    if (count == 3) {
        return 0.86f;
    }
    if (count == 4) {
        return 0.80f;
    }
    if (count <= 8) {
        return 0.72f;
    }
    return 0.62f;
}

static float crowdRadiusForCount(std::size_t count) {
    if (count <= 1) {
        return 0.0f;
    }
    if (count == 2) {
        return 0.18f;
    }
    if (count <= 4) {
        return 0.23f;
    }
    return 0.28f;
}

static Vector3 crowdOffset(std::size_t index, std::size_t count) {
    if (count <= 1) {
        return Vector3{};
    }

    constexpr float halfPi = PI * 0.5f;
    const float radius = crowdRadiusForCount(count);
    const float angle = -halfPi + static_cast<float>(index) * (2.0f * PI / static_cast<float>(count));

    return Vector3{
        std::cos(angle) * radius,
        0.0f,
        std::sin(angle) * radius,
    };
}

static void applyCrowdLayout(std::vector<RenderPlayer> &players) {
    std::map<std::pair<int, int>, std::vector<std::size_t>> groups;

    for (std::size_t i = 0; i < players.size(); i++) {
        groups[tileKey(players[i].position)].push_back(i);
    }

    for (auto &[_, group] : groups) {
        std::sort(group.begin(), group.end(), [&](std::size_t a, std::size_t b) {
            return players[a].playerId < players[b].playerId;
        });

        const float scale = crowdScaleForCount(group.size());
        for (std::size_t slot = 0; slot < group.size(); slot++) {
            RenderPlayer &player = players[group[slot]];
            player.offset = crowdOffset(slot, group.size());
            player.crowdScale = scale;
        }
    }
}

/// Registers skin rendering systems.
MinecraftRenderer::MinecraftRenderer(flecs::world &world) {
    world.module<MinecraftRenderer>("renderer");
    world.component<MinecraftSkin>();

    world.system<const Position3, const Rotation3, const Texture2D, const MinecraftSkin, const SkinPose, const PlayerId>("SkinRender")
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            flecs::world renderWorld = it.world();
            std::vector<RenderPlayer> players;

            while (it.next()) {
                auto positions = it.field<const Position3>(0);
                auto rotations = it.field<const Rotation3>(1);
                auto tex = it.field<const Texture2D>(2);
                auto skin = it.field<const MinecraftSkin>(3);
                auto poses = it.field<const SkinPose>(4);
                auto ids = it.field<const PlayerId>(5);

                for (auto i : it) {
                    players.push_back(RenderPlayer{
                        it.entity(i),
                        std::bit_cast<Vector3>(positions[i]),
                        rotations[i].y,
                        tex[i],
                        skin[i].scale,
                        poses[i],
                        ids[i].value,
                    });
                }
            }

            applyCrowdLayout(players);

            for (const RenderPlayer &player : players) {
                const Vector3 position = {
                    player.position.x + player.offset.x,
                    player.position.y,
                    player.position.z + player.offset.z,
                };
                const float scale = player.scale * player.crowdScale;
                const bool highlighted = shouldHighlightPlayer(renderWorld, player.entity);

                if (!isWithinRenderDistance(renderWorld, position)) {
                    continue;
                }

                if (highlighted) {
                    drawPlayerHoverMarker(position, scale);
                }

                drawPlayer(player.texture, position, scale, player.yaw, player.pose, WHITE);

                if (highlighted) {
                    rlSetBlendMode(BLEND_ADDITIVE);
                    drawPlayer(player.texture, position, scale * 1.006f, player.yaw, player.pose, Color{ 70, 95, 115, 255 });
                    rlSetBlendMode(BLEND_ALPHA);
                }
            }
        });
}
