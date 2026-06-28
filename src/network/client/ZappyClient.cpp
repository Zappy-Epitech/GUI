/**
 * @file ZappyClient.cpp
 * @ingroup gui_network
 * @brief Implements the threaded Zappy client worker loop and queue handling.
 */
#include "ZappyClient.hpp"

#include "../io/LineBuffer.hpp"
#include "ZappySession.hpp"

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

namespace net {
namespace {

/// Builds TCP options tuned for the GUI worker loop.
TcpClientOptions zappyTcpOptions() {
    return {
        .connectTimeout = std::chrono::seconds(1),
        .ioTimeout = std::chrono::milliseconds(50),
        .tcpNoDelay = true,
        .keepAlive = true,
    };
}

} // namespace

/// Stops the network thread before destroying the client.
ZappyClient::~ZappyClient() {
    stop();
}

/// Starts a fresh network worker.
///
/// The socket is owned by the worker thread, not by Flecs. The render/ECS
/// thread only communicates with it through the incoming/outgoing/error queues.
void ZappyClient::start(std::string host, std::uint16_t port) {
    stop();

    {
        std::lock_guard lock(this->mutex);
        this->incoming.clear();
        this->outgoing.clear();
        this->errors.clear();
        this->status = ClientStatus::Connecting;
        this->statusMessage = "Connecting";
    }

    this->thread = std::jthread([this, host = std::move(host), port](std::stop_token stopToken) mutable {
        run(std::move(host), port, stopToken);
    });
}

/// Requests the worker thread to stop and waits for it to exit.
void ZappyClient::stop() {
    if (this->thread.joinable()) {
        this->thread.request_stop();
        this->thread.join();
    }

    setStatus(ClientStatus::Disconnected, "Disconnected");
}

/// Queues a command that will be sent by the network thread.
void ZappyClient::send(std::string command) {
    std::lock_guard lock(this->mutex);
    this->outgoing.push(ZappySession::formatCommand(std::move(command)));
}

/// Pops one complete server line for the ECS thread.
bool ZappyClient::pollLine(std::string &out) {
    std::lock_guard lock(this->mutex);
    return this->incoming.pop(out);
}

/// Pops one network/protocol transport error for the ECS thread.
bool ZappyClient::pollError(std::string &out) {
    std::lock_guard lock(this->mutex);
    return this->errors.pop(out);
}

/// Reads the last known worker status in a thread-safe way.
ClientStatus ZappyClient::getStatus() const {
    std::lock_guard lock(this->mutex);
    return this->status;
}

/// Reads the last known worker status message in a thread-safe way.
std::string ZappyClient::getStatusMessage() const {
    std::lock_guard lock(this->mutex);
    return this->statusMessage;
}

/// Creates the TCP connection with short timeouts.
Result<TcpClient> ZappyClient::createClient(const std::string &host, uint16_t port) {
    return TcpClient::connect(host, port, zappyTcpOptions());
}

/// Main network loop.
void ZappyClient::run(std::string host, std::uint16_t port, std::stop_token stopToken) {
    Result<TcpClient> connection = this->createClient(host, port);

    if (!connection) {
        this->fail(connection.error().message);
        return;
    }

    TcpClient client = std::move(connection.value());
    LineBuffer lines;
    ZappySession session;

    this->setStatus(ClientStatus::Handshaking, "Waiting for WELCOME");

    while (!stopToken.stop_requested() && client.isConnected()) {
        if (!receiveAvailable(client, lines, session)) {
            return;
        }

        if (!flushOutgoing(client)) {
            return;
        }
    }
}

/// Queues one complete server line for the ECS thread.
void ZappyClient::pushLine(std::string line) {
    std::lock_guard lock(this->mutex);
    this->incoming.push(std::move(line));
}

/// Queues one error message for the ECS thread.
void ZappyClient::pushError(std::string message) {
    std::lock_guard lock(this->mutex);
    this->errors.push(std::move(message));
}

/// Pops one command queued by the ECS thread.
bool ZappyClient::popOutgoing(std::string &out) {
    std::lock_guard lock(this->mutex);
    return this->outgoing.pop(out);
}

/// Updates status and message together under the same lock.
void ZappyClient::setStatus(ClientStatus status, std::string message) {
    std::lock_guard lock(this->mutex);
    this->status = status;
    this->statusMessage = std::move(message);
}

/// Records a failure for the ECS thread and marks the client in error.
void ZappyClient::fail(std::string message) {
    this->pushError(message);
    this->setStatus(ClientStatus::Error, std::move(message));
}

/// Receives TCP data, completes lines, and handles the GUI handshake.
bool ZappyClient::receiveAvailable(TcpClient &client, LineBuffer &lines, ZappySession &session) {
    auto chunk = client.receiveString();

    if (!chunk) {
        if (chunk.error().code == ErrorCode::Timeout) {
            return true;
        }
        this->fail(chunk.error().message);
        return false;
    }

    lines.append(*chunk);

    std::string line;
    while (lines.popLine(line)) {
        auto action = session.handleLine(client, line);

        if (!action) {
            this->fail(action.error().message);
            return false;
        }

        if (*action == ZappyLineAction::Connected) {
            this->setStatus(ClientStatus::Connected, "Connected");
            continue;
        }

        if (*action == ZappyLineAction::Forward) {
            this->pushLine(std::move(line));
        }
    }

    return true;
}

/// Flushes all commands queued by the ECS thread to the socket.
bool ZappyClient::flushOutgoing(TcpClient &client) {
    std::string command;

    while (this->popOutgoing(command)) {
        if (auto sent = client.send(command); !sent) {
            this->fail(sent.error().message);
            return false;
        }
    }

    return true;
}

} // namespace net
