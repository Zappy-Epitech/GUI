#include "MinecraftSkinRenderer.hpp"
#include "../extern/flecs.hpp"
#include "Raylib.hpp"
#include "SkinAnimation.hpp"
#include "Spatial.hpp"
#include <bit>
#include <raylib.h>
#include <rlgl.h>

struct FaceUV {
    float x, y, w, h;
};
struct PartUV {
    FaceUV right, front, left, back, top, bottom;
};

static constexpr PartUV HEAD = { { 0, 8, 8, 8 }, { 8, 8, 8, 8 }, { 16, 8, 8, 8 }, { 24, 8, 8, 8 }, { 8, 0, 8, 8 }, { 16, 0, 8, 8 } };
static constexpr PartUV BODY = { { 16, 20, 4, 12 }, { 20, 20, 8, 12 }, { 28, 20, 4, 12 }, { 32, 20, 8, 12 }, { 20, 16, 8, 4 }, { 28, 16, 8, 4 } };
static constexpr PartUV RARM = { { 40, 20, 4, 12 }, { 44, 20, 4, 12 }, { 48, 20, 4, 12 }, { 52, 20, 4, 12 }, { 44, 16, 4, 4 }, { 48, 16, 4, 4 } };
static constexpr PartUV LARM = { { 32, 52, 4, 12 }, { 36, 52, 4, 12 }, { 40, 52, 4, 12 }, { 44, 52, 4, 12 }, { 36, 48, 4, 4 }, { 40, 48, 4, 4 } };
static constexpr PartUV RLEG = { { 0, 20, 4, 12 }, { 4, 20, 4, 12 }, { 8, 20, 4, 12 }, { 12, 20, 4, 12 }, { 4, 16, 4, 4 }, { 8, 16, 4, 4 } };
static constexpr PartUV LLEG = { { 16, 52, 4, 12 }, { 20, 52, 4, 12 }, { 24, 52, 4, 12 }, { 28, 52, 4, 12 }, { 20, 48, 4, 4 }, { 24, 48, 4, 4 } };

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

static void limb(Texture2D tex, const PartUV &uv, float px, float ox, float oy, float oz, float angle, float w, float h, float d) {
    rlPushMatrix();
    rlTranslatef(ox, oy, oz);
    rlRotatef(angle, 1, 0, 0);
    rlTranslatef(0, -h * px * .5f, 0);
    skinCube(tex, uv, w * px, h * px, d * px);
    rlPopMatrix();
}

static void drawPlayer(Texture2D tex, Vector3 pos, float scale, const SkinPose &pose) {
    float px = scale / 16.f;
    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);

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

MinecraftSkinRenderer::MinecraftSkinRenderer(flecs::world &world) {
    world.module<MinecraftSkinRenderer>("MinecraftSkinRenderer");
    world.import<SkinAnim>();
    world.component<MinecraftSkin>();

    static const SkinPose default_pose{};

    world.system<const Position3, const Texture2D, const MinecraftSkin, const SkinPose>("SkinRender")
        .term_at(3)
        .optional()
        .kind<Draw3D>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto pos = it.field<const Position3>(0);
                auto tex = it.field<const Texture2D>(1);
                auto skin = it.field<const MinecraftSkin>(2);
                auto poses = it.field<const SkinPose>(3);
                for (auto i : it)
                    drawPlayer(tex[i], std::bit_cast<Vector3>(pos[i]), skin[i].scale, it.is_set(3) ? poses[i] : default_pose);
            }
        });
}
