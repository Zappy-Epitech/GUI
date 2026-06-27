#include "MinecraftSkinRender.hpp"
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <rlgl.h>

namespace {

/// Stores one face UV rectangle.
struct FaceUV {
    float x, y, w, h;
};

/// Stores UVs for a body part.
struct PartUV {
    FaceUV right, front, left, back, top, bottom;
};

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

    quad({ 0, 0, 1 }, uv.front, { -p.x, -p.y, p.z }, { p.x, -p.y, p.z }, { p.x, p.y, p.z }, { -p.x, p.y, p.z });
    quad({ 0, 0, -1 }, uv.back, { -p.x, -p.y, -p.z }, { -p.x, p.y, -p.z }, { p.x, p.y, -p.z }, { p.x, -p.y, -p.z });
    quad({ 0, 1, 0 }, uv.top, { -p.x, p.y, -p.z }, { -p.x, p.y, p.z }, { p.x, p.y, p.z }, { p.x, p.y, -p.z });
    quad({ 0, -1, 0 }, uv.bottom, { -p.x, -p.y, -p.z }, { p.x, -p.y, -p.z }, { p.x, -p.y, p.z }, { -p.x, -p.y, p.z });
    quad({ 1, 0, 0 }, uv.right, { p.x, p.y, -p.z }, { p.x, p.y, p.z }, { p.x, -p.y, p.z }, { p.x, -p.y, -p.z });
    quad({ -1, 0, 0 }, uv.left, { -p.x, p.y, -p.z }, { -p.x, -p.y, -p.z }, { -p.x, -p.y, p.z }, { -p.x, p.y, p.z });

    rlEnd();
    rlColor4ub(255, 255, 255, 255);
    rlSetTexture(0);
}

static void limb(Texture2D tex, const PartUV &uv, const PartUV &overlayUv, float px, float ox, float oy, float oz, float angle, float w, float h, float d, Color tint) {
    rlPushMatrix();
    rlTranslatef(ox, oy, oz);
    rlRotatef(angle, 1, 0, 0);
    rlTranslatef(0, -h * px * .5f, 0);
    skinCube(tex, uv, w * px, h * px, d * px, tint);
    skinCube(tex, overlayUv, (w + 0.045f) * px, (h + 0.045f) * px, (d + 0.045f) * px, tint);
    rlPopMatrix();
}

static void drawFallbackPlayer(float px, Color tint) {
    const Color body = Color{ 72, 96, 128, tint.a };
    const Color head = Color{ 210, 178, 140, tint.a };

    DrawCube(Vector3{ 0, 6 * px, 0 }, 8 * px, 12 * px, 4 * px, body);
    DrawCube(Vector3{ 0, 16 * px, 0 }, 8 * px, 8 * px, 8 * px, head);
    DrawCube(Vector3{ -6 * px, 6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
    DrawCube(Vector3{ 6 * px, 6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
    DrawCube(Vector3{ -2 * px, -6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
    DrawCube(Vector3{ 2 * px, -6 * px, 0 }, 4 * px, 12 * px, 4 * px, body);
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
    skinCube(tex, HEAD_OVERLAY, 8.45f * px, 8.45f * px, 8.45f * px, WHITE);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(tex, BODY, 8 * px, 12 * px, 4 * px, WHITE);
    skinCube(tex, BODY_OVERLAY, 8.35f * px, 12.35f * px, 4.35f * px, WHITE);
    rlPopMatrix();

    constexpr float relaxedArmAngle = 7.0f;
    limb(tex, RARM, RARM_OVERLAY, px, -6 * px, 12 * px, 0, relaxedArmAngle, 4, 12, 4, WHITE);
    limb(tex, LARM, LARM_OVERLAY, px, 6 * px, 12 * px, 0, -relaxedArmAngle, 4, 12, 4, WHITE);
    limb(tex, RLEG, RLEG_OVERLAY, px, -2 * px, 0, 0, 0, 4, 12, 4, WHITE);
    limb(tex, LLEG, LLEG_OVERLAY, px, 2 * px, 0, 0, 0, 4, 12, 4, WHITE);

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

} // namespace

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

/// Draws a full 3D Minecraft player.
void DrawMinecraftPlayer(Texture2D skin, Vector3 position, float scale, float yaw, const SkinPose &pose, Color tint) {
    const float px = scale / 16.0f;

    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(yaw, 0, 1, 0);

    if (skin.id == 0 || skin.width <= 0 || skin.height <= 0) {
        drawFallbackPlayer(px, tint);
        rlPopMatrix();
        return;
    }

    limb(skin, RLEG, RLEG_OVERLAY, px, -2 * px, 0, 0, pose.angles[(int)Limb::RightLeg], 4, 12, 4, tint);
    limb(skin, LLEG, LLEG_OVERLAY, px, 2 * px, 0, 0, pose.angles[(int)Limb::LeftLeg], 4, 12, 4, tint);

    rlPushMatrix();
    rlRotatef(pose.bodyPitch, 1, 0, 0);

    limb(skin, RARM, RARM_OVERLAY, px, -6 * px, 12 * px, 0, pose.angles[(int)Limb::RightArm], 4, 12, 4, tint);
    limb(skin, LARM, LARM_OVERLAY, px, 6 * px, 12 * px, 0, pose.angles[(int)Limb::LeftArm], 4, 12, 4, tint);

    rlPushMatrix();
    rlTranslatef(0, 16 * px, 0);
    skinCube(skin, HEAD, 8 * px, 8 * px, 8 * px, tint);
    skinCube(skin, HEAD_OVERLAY, 8.45f * px, 8.45f * px, 8.45f * px, tint);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(0, 6 * px, 0);
    skinCube(skin, BODY, 8 * px, 12 * px, 4 * px, tint);
    skinCube(skin, BODY_OVERLAY, 8.35f * px, 12.35f * px, 4.35f * px, tint);
    rlPopMatrix();

    rlPopMatrix();
    rlPopMatrix();
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
    drawPreviewPlayer(skin, 1.0f, lookX * 7.0f, lookX * 42.0f, lookY * 26.0f);
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
