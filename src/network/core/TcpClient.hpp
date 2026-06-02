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

/// Configuration used by TcpClient for connect/read/write operations.
struct TcpClientOptions {
    /// Maximum time spent waiting for a TCP connection attempt.
    std::chrono::milliseconds connectTimeout{ std::chrono::seconds{ 5 } };
    /// Maximum time spent waiting for socket readability/writability.
    std::chrono::milliseconds ioTimeout{ std::chrono::seconds{ 5 } };
    /// Disables Nagle's algorithm for lower latency protocol commands.
    bool tcpNoDelay{ true };
    /// Enables TCP keepalive at the OS level.
    bool keepAlive{ true };
};

/// Small RAII wrapper around a POSIX TCP socket.
///
/// The socket is configured as non-blocking, but public send/receive methods
/// still wait with poll() up to the configured timeout. Use it from a worker
/// thread when the caller must never block rendering.
class TcpClient {
  public:
    TcpClient() = default;
    ~TcpClient() = default;

    TcpClient(const TcpClient &) = delete;
    TcpClient &operator=(const TcpClient &) = delete;

    TcpClient(TcpClient &&other) noexcept = default;
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
    explicit TcpClient(SocketHandle socket, TcpClientOptions options) noexcept;

    SocketHandle socket;
    TcpClientOptions options = {};
};

/// Views a string as immutable bytes without copying.
std::span<const std::byte> asBytes(std::string_view text) noexcept;
/// Copies bytes into a string.
std::string toString(std::span<const std::byte> bytes);

} // namespace net
