#include "MinecraftAnimation.hpp"
#include "src/extern/flecs.h"
#include "src/gameplay/Movement.hpp"
#include <cmath>
#include <raylib.h>

/// Creates an animation clip.
SkinAnimation SkinAnimation::build(float duration, bool loop) {
    SkinAnimation anim{};
    anim.duration = duration;
    anim.loop = loop;
    return anim;
}

/// Adds keyframes for one limb.
SkinAnimation &SkinAnimation::limb(Limb l, std::initializer_list<Keyframe> keys) {
    LimbTrack &track = tracks[static_cast<uint8_t>(l)];
    track.count = 0;
    for (const Keyframe &k : keys) {
        if (track.count >= MAX_KEYFRAMES)
            break;
        track.keys[track.count++] = k;
    }
    return *this;
}

/// Interpolates a limb angle.
static float lerp_limb(const LimbTrack &track, float t) {
    if (track.count == 0)
        return 0.0f;
    if (track.count == 1)
        return track.keys[0].angle;

    for (uint8_t i = 0; i < track.count - 1; i++) {
        const Keyframe &a = track.keys[i];
        const Keyframe &b = track.keys[i + 1];
        if (t >= a.time && t <= b.time) {
            float f = (t - a.time) / (b.time - a.time);
            return a.angle + f * (b.angle - a.angle);
        }
    }
    return track.keys[track.count - 1].angle;
}

/// Registers skin animation systems.
MinecraftAnimation::MinecraftAnimation(flecs::world &world) {
    world.module<MinecraftAnimation>("animation");

    world.component<AnimPlayer>();
    world.component<SkinPose>();

    world.component<SkinAnimation>()
        .add(flecs::With, world.id<AnimPlayer>())
        .add(flecs::With, world.id<SkinPose>());

    world.system<AnimPlayer, SkinPose, const SkinAnimation>("AnimUpdate")
        .kind(flecs::OnUpdate)
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto players = it.field<AnimPlayer>(0);
                auto poses = it.field<SkinPose>(1);
                auto animations = it.field<const SkinAnimation>(2);

                for (auto i : it) {
                    AnimPlayer &player = players[i];
                    SkinPose &pose = poses[i];

                    if (!it.entity(i).has<Direction>()) {
                        player.timer = 0.0f;
                        pose.bodyPitch = 0.0f;
                        for (float &angle : pose.angles) {
                            angle = 0.0f;
                        }
                        continue;
                    }

                    const SkinAnimation &anim = animations[i];
                    player.timer += GetFrameTime() * player.speed;
                    if (anim.loop)
                        player.timer = fmodf(player.timer, anim.duration);
                    else if (player.timer > anim.duration)
                        player.timer = anim.duration;

                    const float t = player.timer;
                    pose.bodyPitch = 0.0f;
                    for (int limb = 0; limb < 4; limb++)
                        pose.angles[limb] = lerp_limb(anim.tracks[limb], t);
                }
            }
        });
}
