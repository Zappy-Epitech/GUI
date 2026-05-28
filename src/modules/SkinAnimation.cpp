#include "SkinAnimation.hpp"
#include "../extern/flecs.hpp"
#include <cmath>
#include <raylib.h>

SkinAnimation SkinAnimation::build(float duration, bool loop) {
    SkinAnimation anim{};
    anim.duration = duration;
    anim.loop = loop;
    return anim;
}

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

SkinAnim::SkinAnim(flecs::world &world) {
    world.module<SkinAnim>("SkinAnim");

    world.component<AnimPlayer>();
    world.component<SkinPose>();

    world.component<SkinAnimation>()
        .add(flecs::With, world.id<AnimPlayer>())
        .add(flecs::With, world.id<SkinPose>());

    world.system<AnimPlayer, SkinPose, const SkinAnimation>("AnimUpdate")
        .kind(flecs::OnUpdate)
        .each([](AnimPlayer &player, SkinPose &pose, const SkinAnimation &anim) {
            player.timer += GetFrameTime() * player.speed;
            if (anim.loop)
                player.timer = fmodf(player.timer, anim.duration);
            else if (player.timer > anim.duration)
                player.timer = anim.duration;

            const float t = player.timer;
            for (int i = 0; i < 4; i++)
                pose.angles[i] = lerp_limb(anim.tracks[i], t);
        });
}
