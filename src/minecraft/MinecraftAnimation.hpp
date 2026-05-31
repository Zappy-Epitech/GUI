#pragma once
#include <cstdint>
#include <initializer_list>

namespace flecs {
struct world;
}

/// Maximum keyframes per limb.
static constexpr int MAX_KEYFRAMES = 8;

/// Identifies an animated limb.
enum class Limb : uint8_t {
    LeftArm = 0,
    RightArm = 1,
    LeftLeg = 2,
    RightLeg = 3,
};

/// Stores one animation keyframe.
struct Keyframe {
    float time;
    float angle;
};

/// Stores keyframes for one limb.
struct LimbTrack {
    Keyframe keys[MAX_KEYFRAMES];
    uint8_t count = 0;
};

/// Stores a skin animation clip.
struct SkinAnimation {
    LimbTrack tracks[4];
    float duration = 1.0f;
    bool loop = true;

    /// Creates an animation clip.
    static SkinAnimation build(float duration, bool loop = true);
    /// Adds keyframes for one limb.
    SkinAnimation &limb(Limb l, std::initializer_list<Keyframe> keys);
};

/// Stores animation playback state.
struct AnimPlayer {
    float timer = 0.0f;
    float speed = 1.0f;
};

/// Stores current limb angles.
struct SkinPose {
    float angles[4] = {};
};

/// Registers skin animation systems.
struct MinecraftAnimation {
    /// Imports the animation module.
    MinecraftAnimation(flecs::world &world);
};

/// Default walking animation.
inline const SkinAnimation Walking = SkinAnimation::build(0.8f)
                                         .limb(Limb::LeftArm, { { 0.0f, 0 }, { 0.4f, 45 }, { 0.8f, 0 } })
                                         .limb(Limb::RightArm, { { 0.0f, 0 }, { 0.4f, -45 }, { 0.8f, 0 } })
                                         .limb(Limb::LeftLeg, { { 0.0f, -30 }, { 0.4f, 30 }, { 0.8f, -30 } })
                                         .limb(Limb::RightLeg, { { 0.0f, 30 }, { 0.4f, -30 }, { 0.8f, 30 } });
