/**
 * @file NetworkModule.cpp
 * @ingroup gui_network
 * @brief Implements the network Flecs module, drain system, and connect/disconnect helpers.
 */
#include "NetworkModule.hpp"

#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Raylib.hpp"
#include "src/core/Scenes.hpp"
#include "src/extern/flecs.h"
#include "src/protocol/command/CommandRunner.hpp"
#include "src/scenes/Home.hpp"

#include <cstdio>
#include <exception>
#include <format>
#include <raylib.h>
#include <string>

namespace {

/// Converts the internal status enum to text for the small on-screen debug UI.
const char *statusLabel(net::ClientStatus status) {
    switch (status) {
    case net::ClientStatus::Disconnected:
        return "Disconnected";
    case net::ClientStatus::Connecting:
        return "Connecting";
    case net::ClientStatus::Handshaking:
        return "Handshaking";
    case net::ClientStatus::Connected:
        return "Connected";
    case net::ClientStatus::Error:
        return "Error";
    }
    return "Unknown";
}

/// Chooses a visible color for each connection state.
Color statusColor(net::ClientStatus status) {
    switch (status) {
    case net::ClientStatus::Connected:
        return GREEN;
    case net::ClientStatus::Connecting:
    case net::ClientStatus::Handshaking:
        return YELLOW;
    case net::ClientStatus::Error:
        return RED;
    case net::ClientStatus::Disconnected:
        return LIGHTGRAY;
    }
    return LIGHTGRAY;
}

} // namespace

/// Registers the network singletons and systems.
///
/// NetworkModule is the bridge between the threaded ZappyClient and Flecs:
/// the worker thread owns the socket, while this module applies received
/// protocol lines on the main ECS thread.
NetworkModule::NetworkModule(flecs::world &world) {
    auto module = world.module<NetworkModule>("network");

    // Singletons make connection config/state available to scenes and systems.
    world.component<NetworkConfig>()
        .member(flecs::String, "host")
        .member<std::uint16_t>("port")
        .set(NetworkConfig("localhost", 6667))
        .child_of(module);

    world.component<NetworkState>()
        .member<int>("status")
        .member(flecs::String, "message")
        .set(NetworkState(net::ClientStatus::Disconnected, "Disconnected"))
        .child_of(module);

    world.component<NetworkClientHandle>()
        .set(NetworkClientHandle());

    world.system("NetworkDrain")
        .read<NetworkClientHandle>()
        .kind(flecs::PreUpdate)
        .immediate(true)
        .run([](flecs::iter &it) {
            flecs::world world = it.world();
            auto &handle = world.get_mut<NetworkClientHandle>();
            if (!handle.client) {
                return;
            }

            // Mirror the worker state into Flecs so UI systems can read it
            // without touching the threaded client directly.
            auto &state = world.get_mut<NetworkState>();
            state.status = handle.client->getStatus();
            state.message = strdup(handle.client->getStatusMessage().c_str());

            // Network errors are queued by the worker and displayed on the ECS
            // thread because creating entities is not thread-safe here.
            std::string error;
            while (handle.client->pollError(error)) {
                state.status = net::ClientStatus::Error;
                state.message = strdup(error.c_str());
                world.entity()
                    .set(ScreenMessage{ error })
                    .set<Color>(RED)
                    .set(Lifetime{ 4.0f });
                enterScene<Home>(world);
            }

            std::string line;
            int budget = 2048;
            // Apply at most a fixed number of server lines per frame. This
            // prevents a large network burst from monopolizing one render tick.
            while (budget-- > 0 && handle.client->pollLine(line)) {
                try {
                    world.defer_suspend();
                    runCommand(world, line);
                    world.defer_resume();
                } catch (const std::exception &err) {
                    world.entity()
                        .set(ScreenMessage{ std::format("Protocol error: {}", err.what()) })
                        .set<Color>(RED)
                        .set(Lifetime{ 4.0f });
                }
            }
        });

    // Lightweight status overlay. It is intentionally simple because the real
    // network state lives in NetworkState and can be rendered elsewhere later.
    world.system<const NetworkState>("DrawNetworkStatus")
        .kind<Render2D>()
        .run([](flecs::iter &it) {
            while (it.next()) {
                auto states = it.field<const NetworkState>(0);
                for (auto i : it) {
                    const std::string text = std::format("Status: {}", statusLabel(states[i].status));
                    DrawText(text.c_str(), 20, GetScreenHeight() - 36, 24, statusColor(states[i].status));
                }
            }
        });
}

/// Starts a new connection using the shared net::ZappyClient.
///
/// Reusing the same handle lets scenes call connect/disconnect without owning
/// the thread object directly.
void connectToServer(flecs::world &world, const NetworkConfig &config) {
    auto &handle = world.get_mut<NetworkClientHandle>();
    if (!handle.client) {
        handle.client = std::make_shared<net::ZappyClient>();
    }

    world.set<NetworkConfig>(config);
    world.set<NetworkState>(NetworkState(net::ClientStatus::Connecting, std::format("{}:{}", config.host, config.port).c_str()));
    handle.client->start(config.host, config.port);
}

/// Stops the active connection and updates the ECS-visible state.
void disconnectFromServer(flecs::world &world) {
    auto &handle = world.get_mut<NetworkClientHandle>();
    if (handle.client) {
        handle.client->stop();
    }
    world.set<NetworkState>(NetworkState(net::ClientStatus::Disconnected, "Disconnected"));
}

/// Sends a command through the active network client.
bool sendServerCommand(const flecs::world &world, std::string_view command) {
    auto &handle = world.get_mut<NetworkClientHandle>();
    if (!handle.client || handle.client->getStatus() != net::ClientStatus::Connected) {
        return false;
    }

    handle.client->send(std::string(command));
    return true;
}
