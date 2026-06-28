/**
 * @file SocketConnector.hpp
 * @ingroup gui_network
 * @brief Opens, configures, and connects a socket for one resolved address.
 */
#pragma once

#include "../utils/Result.hpp"
#include "SocketHandle.hpp"

/// Forward declaration for one getaddrinfo() result node.
struct addrinfo;

namespace net {

/// Forward declaration for TCP timeout and option settings.
struct TcpClientOptions;

/// Opens, configures, and connects a socket for one resolved address.
Result<SocketHandle> connectSocket(const addrinfo &address, const TcpClientOptions &options);

} // namespace net
