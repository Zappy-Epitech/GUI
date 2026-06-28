/**
 * @file NetworkModule.hpp
 * @ingroup gui_network
 * @brief Flecs module and helpers bridging the threaded client into the ECS world.
 */
#pragma once

#include "client/ZappyClient.hpp"

#include <cstdint>
#include <cstring>
#include <string_view>

namespace flecs {
/// Forward declaration for the ECS world type.
struct world;
} // namespace flecs

/// Host/port chosen by the menu before entering the game scene.
struct NetworkConfig {
    /// Server hostname copied for Flecs storage.
    char *host = nullptr;
    /// Server TCP port.
    std::uint16_t port = 4242;

    /// Builds the default localhost configuration.
    NetworkConfig() {
        this->host = strdup("localhost");
    }

    /// Copies an explicit host/port pair.
    NetworkConfig(const char *host, std::uint16_t port) {
        this->host = strdup(host);
        this->port = port;
    }

    /// Releases the duplicated host string.
    ~NetworkConfig() {
        free(this->host);
    }
};

/// Last known client state mirrored into Flecs for rendering and systems.
struct NetworkState {
    /// Current connection state.
    net::ClientStatus status = net::ClientStatus::Disconnected;
    /// Human-readable status details.
    char *message = nullptr;

    /// Builds the default disconnected state.
    NetworkState() {
        this->message = strdup("Disconnected");
    }

    /// Copies a status and display message.
    NetworkState(net::ClientStatus status, const char *message) {
        this->status = status;
        this->message = strdup(message);
    }

    /// Releases the duplicated status message.
    ~NetworkState() {
        free(this->message);
    }
};

/// Flecs singleton that owns the shared threaded client.
struct NetworkClientHandle {
    /// Shared client used by network systems and scenes.
    net::ZappyClientPtr client = nullptr;
};

/** @brief Flecs module bridging the threaded ZappyClient into the ECS world.
 *
 * @details Registers the network singleton components (NetworkConfig,
 * NetworkState, NetworkClientHandle) and the per-frame NetworkDrain system,
 * which runs in PreUpdate to mirror worker status into NetworkState and drain
 * the FIFO error and incoming-line queues (dispatching protocol lines to the
 * command runner). Also registers the DrawNetworkStatus overlay.
 * @ingroup gui_network
 */
struct NetworkModule {
    /// Installs the network module into the Flecs world.
    NetworkModule(flecs::world &world);
};

/// Creates or reuses the client and starts a connection attempt.
void connectToServer(flecs::world &world, const NetworkConfig &config);
/// Stops the active client and marks the network state as disconnected.
void disconnectFromServer(flecs::world &world);
/// Sends a command to the active server when the client is connected.
bool sendServerCommand(const flecs::world &world, std::string_view command);
