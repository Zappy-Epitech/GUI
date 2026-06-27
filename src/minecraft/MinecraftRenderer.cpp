#include "MinecraftRenderer.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Settings.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Team.hpp"
#include "src/minecraft/MinecraftAnimation.hpp"
#include "src/scenes/GameUi.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <raylib.h>
#include <rlgl.h>

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

/// Draws the front face of a Minecraft skin head in 2D.
void DrawMinecraftHead(Texture2D skin, Rectangle bounds) {
    if (skin.id == 0 || skin.width <= 0 || skin.height <= 0) {
        DrawRectangleRec(bounds, Fade(GRAY, 0.65f));
        DrawRectangleLinesEx(bounds, 1.0f, Fade(WHITE, 0.45f));
        return;
    }

    const float px = static_cast<float>(skin.width) / 64.0f;
    const Rectangle head = { 8.0f * px, 8.0f * px, 8.0f * px, 8.0f * px };
    const Rectangle overlay = { 40.0f * px, 8.0f * px, 8.0f * px, 8.0f * px };

    DrawTexturePro(skin, head, bounds, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    if (skin.width >= static_cast<int>(48.0f * px) && skin.height >= static_cast<int>(16.0f * px)) {
        DrawTexturePro(skin, overlay, bounds, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLinesEx(bounds, 1.0f, Fade(BLACK, 0.35f));
}

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
static void limb(Texture2D tex, const PartUV &uv, float px, float ox, float oy, float oz, float angle, float w, float h, float d, Color tint) {
    rlPushMatrix();
    rlTranslatef(ox, oy, oz);
    rlRotatef(angle, 1, 0, 0);
    rlTranslatef(0, -h * px * .5f, 0);
    skinCube(tex, uv, w * px, h * px, d * px, tint);
    rlPopMatrix();
}

/// Draws a Minecraft-style player.
static void drawPlayer(Texture2D tex, Vector3 pos, float scale, float angle, const SkinPose &pose, Color tint) {
    float px = scale / 16.f;
    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);
    rlRotatef(angle, 0, 1, 0);

    limb(tex, RLEG, px, -2 * px, 0, 0, pose.angles[(int)Limb::RightLeg], 4, 12, 4, tint);
    limb(tex, LLEG, px, 2 * px, 0, 0, pose.angles[(int)Limb::LeftLeg], 4, 12, 4, tint);

    rlPushMatrix();
    rlTranslatef(0, 0, 0);
    rlRotatef(pose.bodyPitch, 1, 0, 0);

    limb(tex, RARM, px, -6 * px, 12 * px, 0, pose.angles[(int)Limb::RightArm], 4, 12, 4, tint);
    limb(tex, LARM, px, 6 * px, 12 * px, 0, pose.angles[(int)Limb::LeftArm], 4, 12, 4, tint);

    rlPushMatrix();
    rlTranslatef(0, 16 * px, 0);
    skinCube(tex, HEAD, 8 * px, 8 * px, 8 * px, tint);
    rlPopMatrix();
    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(tex, BODY, 8 * px, 12 * px, 4 * px, tint);
    rlPopMatrix();

    rlPopMatrix();

    rlPopMatrix();
}

static void drawPreviewPlayer(Texture2D tex, float scale, float bodyYaw, float headYaw, float headPitch) {
    const float px = scale / 16.0f;

    rlPushMatrix();
    rlRotatef(bodyYaw, 0, 1, 0);

    rlPushMatrix();
    rlTranslatef(0, 16 * px, 0);
    rlRotatef(headYaw, 0, 1, 0);
    rlRotatef(headPitch, 1, 0, 0);
    skinCube(tex, HEAD, 8 * px, 8 * px, 8 * px, WHITE);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(tex, BODY, 8 * px, 12 * px, 4 * px, WHITE);
    rlPopMatrix();

    constexpr float relaxedArmAngle = 7.0f;
    limb(tex, RARM, px, -6 * px, 12 * px, 0, relaxedArmAngle, 4, 12, 4, WHITE);
    limb(tex, LARM, px, 6 * px, 12 * px, 0, -relaxedArmAngle, 4, 12, 4, WHITE);
    limb(tex, RLEG, px, -2 * px, 0, 0, 0, 4, 12, 4, WHITE);
    limb(tex, LLEG, px, 2 * px, 0, 0, 0, 4, 12, 4, WHITE);

    rlPopMatrix();
}

static RenderTexture2D &previewTexture(int width, int height) {
    static RenderTexture2D target = {};

    if (target.id == 0 || target.texture.width != width || target.texture.height != height) {
        if (target.id != 0) {
            UnloadRenderTexture(target);
        }
        target = LoadRenderTexture(width, height);
        SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
    }
    return target;
}

/// Draws a 3D inventory player preview whose head tracks the mouse.
void DrawMinecraftPlayerPreview3D(Texture2D skin, Rectangle bounds, Vector2 mouse) {
    if (skin.id == 0 || skin.width <= 0 || skin.height <= 0) {
        DrawRectangleRec(bounds, Fade(GRAY, 0.35f));
        DrawRectangleLinesEx(bounds, 1.0f, Fade(WHITE, 0.35f));
        return;
    }

    const int textureWidth = std::max(96, static_cast<int>(std::round(bounds.width)));
    const int textureHeight = std::max(128, static_cast<int>(std::round(bounds.height)));
    RenderTexture2D &target = previewTexture(textureWidth, textureHeight);

    const Vector2 center = {
        bounds.x + bounds.width * 0.5f,
        bounds.y + bounds.height * 0.36f,
    };
    const float lookX = std::clamp((mouse.x - center.x) / std::max(bounds.width * 0.75f, 1.0f), -1.0f, 1.0f);
    const float lookY = std::clamp((mouse.y - center.y) / std::max(bounds.height * 0.65f, 1.0f), -1.0f, 1.0f);
    const float headYaw = lookX * 42.0f;
    const float headPitch = lookY * 26.0f;
    const float bodyYaw = lookX * 7.0f;
    const Camera3D camera = {
        Vector3{ 0.0f, 0.9f, 7.0f },
        Vector3{ 0.0f, 0.42f, 0.0f },
        Vector3{ 0.0f, 1.0f, 0.0f },
        19.0f,
        CAMERA_PERSPECTIVE,
    };

    BeginTextureMode(target);
    ClearBackground(BLANK);
    BeginMode3D(camera);
    rlDisableBackfaceCulling();
    drawPreviewPlayer(skin, 1.0f, bodyYaw, headYaw, headPitch);
    rlEnableBackfaceCulling();
    EndMode3D();
    EndTextureMode();

    DrawTexturePro(target.texture,
                   Rectangle{ 0.0f, 0.0f, static_cast<float>(target.texture.width), -static_cast<float>(target.texture.height) },
                   bounds,
                   Vector2{ 0.0f, 0.0f },
                   0.0f,
                   WHITE);
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

/// Registers skin rendering systems.
MinecraftRenderer::MinecraftRenderer(flecs::world &world) {
    world.module<MinecraftRenderer>("renderer");
    world.component<MinecraftSkin>();

    world.system<const Position3, const Rotation3, const Texture2D, const MinecraftSkin, const SkinPose>("SkinRender")
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto positions = it.field<const Position3>(0);
                auto rotations = it.field<const Rotation3>(1);
                auto tex = it.field<const Texture2D>(2);
                auto skin = it.field<const MinecraftSkin>(3);
                auto poses = it.field<const SkinPose>(4);

                for (auto i : it) {
                    const Vector3 position = std::bit_cast<Vector3>(positions[i]);

                    const bool highlighted = shouldHighlightPlayer(it.world(), it.entity(i));

                    if (highlighted) {
                        drawPlayerHoverMarker(position, skin[i].scale);
                    }

                    drawPlayer(tex[i], position, skin[i].scale, rotations[i].y, poses[i], WHITE);

                    if (highlighted) {
                        rlSetBlendMode(BLEND_ADDITIVE);
                        drawPlayer(tex[i], position, skin[i].scale * 1.006f, rotations[i].y, poses[i], Color{ 70, 95, 115, 255 });
                        rlSetBlendMode(BLEND_ALPHA);
                    }
                }
            }
        });
}
