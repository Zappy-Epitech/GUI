#pragma once

#include "../utils/Result.hpp"
#include "SocketHandle.hpp"

struct addrinfo;

namespace net {

struct TcpClientOptions;

Result<SocketHandle> connectSocket(const addrinfo &address, const TcpClientOptions &options);

} // namespace net
