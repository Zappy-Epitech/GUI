#pragma once
#include <cstdint>
#include <initializer_list>

namespace flecs {
struct world;
}

static constexpr int MAX_KEYFRAMES = 8;

enum class Limb : uint8_t {
    LeftArm = 0,
    RightArm = 1,
    LeftLeg = 2,
    RightLeg = 3,
};

struct Keyframe {
    float time;
    float angle;
};

struct LimbTrack {
    Keyframe keys[MAX_KEYFRAMES];
    uint8_t count = 0;
};

struct SkinAnimation {
    LimbTrack tracks[4];
    float duration = 1.0f;
    bool loop = true;

    static SkinAnimation build(float duration, bool loop = true);
    SkinAnimation &limb(Limb l, std::initializer_list<Keyframe> keys);
};

struct AnimPlayer {
    float timer = 0.0f;
    float speed = 1.0f;
};

struct SkinPose {
    float angles[4] = {};
};

struct MinecraftAnimation {
    MinecraftAnimation(flecs::world &world);
};

inline const SkinAnimation Walking = SkinAnimation::build(0.8f)
                                         .limb(Limb::LeftArm, { { 0.0f, 0 }, { 0.4f, 45 }, { 0.8f, 0 } })
                                         .limb(Limb::RightArm, { { 0.0f, 0 }, { 0.4f, -45 }, { 0.8f, 0 } })
                                         .limb(Limb::LeftLeg, { { 0.0f, -30 }, { 0.4f, 30 }, { 0.8f, -30 } })
                                         .limb(Limb::RightLeg, { { 0.0f, 30 }, { 0.4f, -30 }, { 0.8f, 30 } });
