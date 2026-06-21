#include "MinecraftRenderer.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/minecraft/MinecraftAnimation.hpp"
#include <bit>
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
static void skinCube(Texture2D tex, const PartUV &uv, float w, float h, float d) {
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
    rlSetTexture(0);
}

/// Draws one animated limb.
static void limb(Texture2D tex, const PartUV &uv, float px, float ox, float oy, float oz, float angle, float w, float h, float d) {
    rlPushMatrix();
    rlTranslatef(ox, oy, oz);
    rlRotatef(angle, 1, 0, 0);
    rlTranslatef(0, -h * px * .5f, 0);
    skinCube(tex, uv, w * px, h * px, d * px);
    rlPopMatrix();
}

/// Draws a Minecraft-style player.
static void drawPlayer(Texture2D tex, Vector3 pos, float scale, float angle, const SkinPose &pose) {
    float px = scale / 16.f;
    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);
    rlRotatef(angle, 0, 1, 0);

    rlPushMatrix();
    rlTranslatef(0, 16 * px, 0);
    skinCube(tex, HEAD, 8 * px, 8 * px, 8 * px);
    rlPopMatrix();
    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(tex, BODY, 8 * px, 12 * px, 4 * px);
    rlPopMatrix();

    limb(tex, RARM, px, -6 * px, 12 * px, 0, pose.angles[(int)Limb::RightArm], 4, 12, 4);
    limb(tex, LARM, px, 6 * px, 12 * px, 0, pose.angles[(int)Limb::LeftArm], 4, 12, 4);
    limb(tex, RLEG, px, -2 * px, 0, 0, pose.angles[(int)Limb::RightLeg], 4, 12, 4);
    limb(tex, LLEG, px, 2 * px, 0, 0, pose.angles[(int)Limb::LeftLeg], 4, 12, 4);

    rlPopMatrix();
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

                for (auto i : it)
                    drawPlayer(tex[i], std::bit_cast<Vector3>(positions[i]), skin[i].scale, rotations[i].y, poses[i]);
            }
        });
}
