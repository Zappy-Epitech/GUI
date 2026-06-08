#include "TcpClient.hpp"
#include "AdressList.hpp"
#include "SocketConnector.hpp"
#include "SocketPoll.hpp"

#include <cerrno>
#include <sys/socket.h>
#include <utility>

namespace net {

/// Takes ownership of an already connected socket descriptor.
TcpClient::TcpClient(SocketHandle socket, TcpClientOptions options) noexcept
    : socket(std::move(socket)), options(options) {
}

/// Resolves every address for host:port and tries them until one connects.
Result<TcpClient> TcpClient::connect(std::string_view host, std::uint16_t port, TcpClientOptions options) {
    AddressList addresses(host, port);

    if (addresses.getError() != 0) {
        return std::unexpected(makeDnsError(addresses.getError(), "getaddrinfo"));
    }

    Error lastError = makeInvalidStateError("no address found");

    for (addrinfo *address = addresses.begin(); address != nullptr; address = address->ai_next) {
        auto connected = connectSocket(*address, options);

        if (connected) {
            return TcpClient(std::move(*connected), options);
        }

        lastError = connected.error();
    }

    return std::unexpected(lastError);
}

/// Returns whether this wrapper owns a valid descriptor.
bool TcpClient::isConnected() const noexcept {
    return socket.valid();
}

/// Returns the raw socket descriptor, or -1 if disconnected.
int TcpClient::nativeHandle() const noexcept {
    return socket.get();
}

/// Sends the full byte span, handling short writes and transient EAGAIN/EINTR.
Result<void> TcpClient::send(std::span<const std::byte> data) {
    if (!isConnected()) {
        return std::unexpected(makeInvalidStateError("send on disconnected client"));
    }

    while (!data.empty()) {
        if (auto wait = waitWritable(socket.get(), options.ioTimeout, "send"); !wait) {
            return wait;
        }

        const ssize_t sent = ::send(socket.get(), data.data(), data.size(), MSG_NOSIGNAL);

        if (sent > 0) {
            data = data.subspan(static_cast<std::size_t>(sent));
            continue;
        }

        if (sent == 0) {
            return std::unexpected(makeDisconnectedError("send"));
        }

        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
            continue;
        }

        return std::unexpected(makeSystemError("send"));
    }

    return {};
}

/// Sends string data as raw bytes.
Result<void> TcpClient::send(std::string_view text) {
    return send(asBytes(text));
}

/// Receives one chunk from the socket.
///
/// This does not try to split protocol messages. TCP is stream based, so the
/// higher layer must buffer and split by newlines.
Result<std::vector<std::byte>> TcpClient::receive(std::size_t maxBytes) {
    if (!isConnected()) {
        return std::unexpected(makeInvalidStateError("receive on disconnected client"));
    }

    std::vector<std::byte> buffer(maxBytes);

    if (auto wait = waitReadable(socket.get(), options.ioTimeout, "receive"); !wait) {
        return std::unexpected(wait.error());
    }

    while (true) {
        const ssize_t received = ::recv(socket.get(), buffer.data(), buffer.size(), 0);

        if (received > 0) {
            buffer.resize(static_cast<std::size_t>(received));
            return buffer;
        }

        if (received == 0) {
            return std::unexpected(makeDisconnectedError("receive"));
        }

        if (errno == EINTR) {
            continue;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (auto wait = waitReadable(socket.get(), options.ioTimeout, "receive"); !wait) {
                return std::unexpected(wait.error());
            }
            continue;
        }

        return std::unexpected(makeSystemError("receive"));
    }
}

/// Receives one chunk and converts it to a string.
Result<std::string> TcpClient::receiveString(std::size_t maxBytes) {
    auto bytes = receive(maxBytes);

    if (!bytes) {
        return std::unexpected(bytes.error());
    }

    return toString(*bytes);
}

/// Closes the socket descriptor. Safe to call multiple times.
void TcpClient::disconnect() noexcept {
    socket.reset();
}

/// Reinterprets string storage as bytes without allocating.
std::span<const std::byte> asBytes(std::string_view text) noexcept {
    return {
        reinterpret_cast<const std::byte *>(text.data()),
        text.size()
    };
}

/// Copies byte storage into a string.
std::string toString(std::span<const std::byte> bytes) {
    return {
        reinterpret_cast<const char *>(bytes.data()),
        bytes.size()
    };
}

} // namespace net
