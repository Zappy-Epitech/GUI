/**
 * @file MinecraftAnimation.hpp
 * @ingroup gui_minecraft
 * @brief Skin animation data (keyframe tracks, poses) and the flecs module that updates them.
 */
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
    float time;  ///< Time of the keyframe, in seconds from the clip start.
    float angle; ///< Limb rotation angle at this keyframe, in degrees.
};

/// Stores keyframes for one limb.
struct LimbTrack {
    Keyframe keys[MAX_KEYFRAMES]; ///< Keyframes in ascending time order.
    uint8_t count = 0;            ///< Number of valid keyframes in @ref keys.
};

/// Stores a skin animation clip.
struct SkinAnimation {
    LimbTrack tracks[4];   ///< One keyframe track per limb, indexed by ::Limb.
    float duration = 1.0f; ///< Total clip length, in seconds.
    bool loop = true;      ///< Whether playback wraps around at @ref duration.

    /// Creates an animation clip.
    static SkinAnimation build(float duration, bool loop = true);
    /// Adds keyframes for one limb.
    SkinAnimation &limb(Limb l, std::initializer_list<Keyframe> keys);
};

/// Stores animation playback state.
struct AnimPlayer {
    float timer = 0.0f; ///< Current playback position within the clip, in seconds.
    float speed = 1.0f; ///< Playback speed multiplier applied each frame.
};

/// Stores current limb angles.
struct SkinPose {
    float angles[4] = {};   ///< Current rotation per limb, in degrees, indexed by ::Limb.
    float bodyPitch = 0.0f; ///< Forward/back pitch of the upper body, in degrees.
};

/**
 * @brief flecs module that registers the skin animation components and update system.
 * @ingroup gui_minecraft
 * @details Registers the SkinAnimation, AnimPlayer and SkinPose components and
 * the @c AnimUpdate system which advances each player's timer and interpolates
 * its limb keyframe tracks into the current SkinPose every frame.
 */
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
