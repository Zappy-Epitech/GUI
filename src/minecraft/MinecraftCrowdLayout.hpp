#pragma once
#include <raylib.h>
#include <vector>

/// Stores one player entry participating in same-tile render layout.
struct MinecraftCrowdMember {
    int playerId;
    Vector3 position;
    Vector3 offset = {};
    float scale = 1.0f;
};

/// Applies stable same-tile offsets and scale factors to the members.
void ApplyMinecraftCrowdLayout(std::vector<MinecraftCrowdMember> &members);
