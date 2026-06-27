#include "MinecraftCrowdLayout.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <raylib.h>
#include <utility>

namespace {

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

} // namespace

/// Applies stable same-tile offsets and scale factors to the members.
void ApplyMinecraftCrowdLayout(std::vector<MinecraftCrowdMember> &members) {
    std::map<std::pair<int, int>, std::vector<std::size_t>> groups;

    for (std::size_t i = 0; i < members.size(); i++) {
        groups[tileKey(members[i].position)].push_back(i);
    }

    for (auto &[_, group] : groups) {
        std::sort(group.begin(), group.end(), [&](std::size_t a, std::size_t b) {
            return members[a].playerId < members[b].playerId;
        });

        const float scale = crowdScaleForCount(group.size());
        for (std::size_t slot = 0; slot < group.size(); slot++) {
            MinecraftCrowdMember &member = members[group[slot]];
            member.offset = crowdOffset(slot, group.size());
            member.scale = scale;
        }
    }
}
