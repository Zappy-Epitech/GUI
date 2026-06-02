#include "SocketOptions.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

namespace net {

Result<void> setNonBlocking(int fd) {
    const int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return std::unexpected(makeSystemError("fcntl(F_GETFL)"));
    }

    if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        return std::unexpected(makeSystemError("fcntl(F_SETFL)"));
    }

    return {};
}

Result<void> setSocketOption(int fd, int level, int name, bool enabled, std::string_view context) {
    const int value = enabled ? 1 : 0;

    if (::setsockopt(fd, level, name, &value, sizeof(value)) < 0) {
        return std::unexpected(makeSystemError(context));
    }

    return {};
}

Result<void> configureTcpSocket(int fd, bool tcpNoDelay, bool keepAlive) {
    if (auto result = setSocketOption(fd, IPPROTO_TCP, TCP_NODELAY, tcpNoDelay, "setsockopt(TCP_NODELAY)"); !result) {
        return result;
    }

    if (auto result = setSocketOption(fd, SOL_SOCKET, SO_KEEPALIVE, keepAlive, "setsockopt(SO_KEEPALIVE)"); !result) {
        return result;
    }

    return setNonBlocking(fd);
}

} // namespace net
