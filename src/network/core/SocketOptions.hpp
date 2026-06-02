#pragma once

#include "../utils/Result.hpp"

#include <string_view>

namespace net {

Result<void> setNonBlocking(int fd);
Result<void> setSocketOption(int fd, int level, int name, bool enabled, std::string_view context);
Result<void> configureTcpSocket(int fd, bool tcpNoDelay, bool keepAlive);

} // namespace net
