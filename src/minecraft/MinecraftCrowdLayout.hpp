/**
 * @file MinecraftCrowdLayout.hpp
 * @ingroup gui_minecraft
 * @brief Spreads players standing on the same tile into a stable ring with scaling.
 */
#pragma once
#include <raylib.h>
#include <vector>

/**
 * @brief One player entry participating in same-tile render layout.
 * @ingroup gui_minecraft
 */
struct MinecraftCrowdMember {
    int playerId;          ///< Stable player identifier used to order ring slots deterministically.
    Vector3 position;      ///< World position used to group members onto tiles (input).
    Vector3 offset = {};   ///< Computed horizontal offset from the tile centre (output).
    float scale = 1.0f;    ///< Computed render scale shrinking crowded tiles (output).
};

/// Applies stable same-tile offsets and scale factors to the members.
void ApplyMinecraftCrowdLayout(std::vector<MinecraftCrowdMember> &members);
