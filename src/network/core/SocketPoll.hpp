#pragma once

#include "../utils/Result.hpp"

#include <chrono>
#include <string_view>

namespace net {

Result<void> checkSocketError(int fd, std::string_view context);
Result<void> waitReadable(int fd, std::chrono::milliseconds timeout, std::string_view context);
Result<void> waitWritable(int fd, std::chrono::milliseconds timeout, std::string_view context);

} // namespace net
