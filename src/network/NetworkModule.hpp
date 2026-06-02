#pragma once

#include "client/ZappyClient.hpp"

#include <cstdint>
#include <cstring>

namespace flecs {
struct world;
}

/// Host/port chosen by the menu before entering the game scene.
struct NetworkConfig {
    char *host = nullptr;
    std::uint16_t port = 4242;

    NetworkConfig() {
        this->host = strdup("localhost");
    }

    NetworkConfig(const char *host, std::uint16_t port) {
        this->host = strdup(host);
        this->port = port;
    }

    ~NetworkConfig() {
        free(this->host);
    }
};

/// Last known client state mirrored into Flecs for rendering and systems.
struct NetworkState {
    net::ClientStatus status = net::ClientStatus::Disconnected;
    char *message = nullptr;

    NetworkState() {
        this->message = strdup("Disconnected");
    }

    NetworkState(net::ClientStatus status, const char *message) {
        this->status = status;
        this->message = strdup(message);
    }

    ~NetworkState() {
        free(this->message);
    }
};

/// Flecs singleton that owns the shared threaded client.
struct NetworkClientHandle {
    net::ZappyClientPtr client = nullptr;
};

/// Registers network components and systems.
struct NetworkModule {
    NetworkModule(flecs::world &world);
};

/// Creates or reuses the client and starts a connection attempt.
void connectToServer(flecs::world &world, const NetworkConfig &config);
/// Stops the active client and marks the network state as disconnected.
void disconnectFromServer(flecs::world &world);
