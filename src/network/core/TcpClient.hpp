/**
 * @file TcpClient.hpp
 * @ingroup gui_network
 * @brief RAII non-blocking TCP client with poll-based timeouts and byte helpers.
 */
#pragma once

#include "../utils/Result.hpp"
#include "SocketHandle.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace net {

/** @brief Configuration used by TcpClient for connect/read/write operations.
 * @ingroup gui_network
 */
struct TcpClientOptions {
    std::chrono::milliseconds connectTimeout{ std::chrono::seconds{ 5 } }; ///< Max wait for a connect() attempt (milliseconds).
    std::chrono::milliseconds ioTimeout{ std::chrono::seconds{ 5 } };      ///< Max wait per send/receive readiness poll (milliseconds).
    bool tcpNoDelay{ true };                                              ///< Disable Nagle's algorithm (TCP_NODELAY) for lower latency.
    bool keepAlive{ true };                                              ///< Enable OS-level TCP keepalive (SO_KEEPALIVE).
};

/** @brief Small RAII wrapper around a POSIX TCP socket.
 *
 * The socket is configured as non-blocking, but public send/receive methods
 * still wait with poll() up to the configured timeout. Use it from a worker
 * thread when the caller must never block rendering.
 * @ingroup gui_network
 */
class TcpClient {
  public:
    /// Creates a disconnected client wrapper.
    TcpClient() = default;
    /// Lets SocketHandle close any owned descriptor.
    ~TcpClient() = default;

    /// Prevents copying the socket owner.
    TcpClient(const TcpClient &) = delete;
    /// Prevents assigning the socket owner.
    TcpClient &operator=(const TcpClient &) = delete;

    /// Moves socket ownership from another client.
    TcpClient(TcpClient &&other) noexcept = default;
    /// Replaces this socket with another client's socket.
    TcpClient &operator=(TcpClient &&other) noexcept = default;

    /// Resolves host, connects to port, configures socket options, and returns
    /// a connected TcpClient on success.
    static Result<TcpClient> connect(
        std::string_view host,
        std::uint16_t port,
        TcpClientOptions options = {});

    /// Returns true when this wrapper currently owns a socket descriptor.
    [[nodiscard]] bool isConnected() const noexcept;
    /// Exposes the underlying file descriptor for integration code.
    [[nodiscard]] int nativeHandle() const noexcept;

    /// Sends all bytes before returning, unless an error or timeout occurs.
    Result<void> send(std::span<const std::byte> data);
    /// Sends UTF-8/text protocol data as raw bytes.
    Result<void> send(std::string_view text);

    /// Receives one available chunk, up to maxBytes.
    Result<std::vector<std::byte>> receive(std::size_t maxBytes = 64 * 1024);
    /// Receives one available chunk and converts it to std::string.
    Result<std::string> receiveString(std::size_t maxBytes = 64 * 1024);

    /// Closes the socket if one is currently owned.
    void disconnect() noexcept;

  private:
    /// Takes ownership of a connected socket.
    explicit TcpClient(SocketHandle socket, TcpClientOptions options) noexcept;

    /// Owned socket descriptor.
    SocketHandle socket;
    /// Timeouts and TCP options used by operations.
    TcpClientOptions options = {};
};

/// Views a string as immutable bytes without copying.
std::span<const std::byte> asBytes(std::string_view text) noexcept;
/// Copies bytes into a string.
std::string toString(std::span<const std::byte> bytes);

} // namespace net
