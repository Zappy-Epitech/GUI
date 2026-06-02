#pragma once

#include "../utils/Result.hpp"

#include <string_view>

namespace net {

/// Puts the socket descriptor in non-blocking mode.
Result<void> setNonBlocking(int fd);
/// Enables or disables one boolean socket option.
Result<void> setSocketOption(int fd, int level, int name, bool enabled, std::string_view context);
/// Applies the socket options used by TcpClient.
Result<void> configureTcpSocket(int fd, bool tcpNoDelay, bool keepAlive);

} // namespace net
