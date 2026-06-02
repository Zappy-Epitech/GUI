#include "SocketConnector.hpp"
#include "SocketOptions.hpp"
#include "SocketPoll.hpp"
#include "TcpClient.hpp"

#include <cerrno>
#include <netdb.h>
#include <sys/socket.h>

namespace net {

/// Opens, configures, and connects a socket for one resolved address.
Result<SocketHandle> connectSocket(const addrinfo &address, const TcpClientOptions &options) {
    SocketHandle socket(::socket(address.ai_family, address.ai_socktype, address.ai_protocol));

    if (!socket.valid()) {
        return std::unexpected(makeSystemError("socket"));
    }

    if (auto result = configureTcpSocket(socket.get(), options.tcpNoDelay, options.keepAlive); !result) {
        return std::unexpected(result.error());
    }

    const int result = ::connect(socket.get(), address.ai_addr, address.ai_addrlen);

    if (result == 0) {
        return socket;
    }

    if (errno != EINPROGRESS) {
        return std::unexpected(makeSystemError("connect"));
    }

    if (auto wait = waitWritable(socket.get(), options.connectTimeout, "connect"); !wait) {
        return std::unexpected(wait.error());
    }

    if (auto checked = checkSocketError(socket.get(), "connect"); !checked) {
        return std::unexpected(checked.error());
    }

    return socket;
}

} // namespace net
