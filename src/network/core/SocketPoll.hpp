/**
 * @file SocketPoll.hpp
 * @ingroup gui_network
 * @brief poll()-based readiness waits and asynchronous socket error checks.
 */
#pragma once

#include "../utils/Result.hpp"

#include <chrono>
#include <string_view>

namespace net {

/// Returns any pending asynchronous socket error.
Result<void> checkSocketError(int fd, std::string_view context);
/// Waits until a socket can be read.
Result<void> waitReadable(int fd, std::chrono::milliseconds timeout, std::string_view context);
/// Waits until a socket can be written.
Result<void> waitWritable(int fd, std::chrono::milliseconds timeout, std::string_view context);

} // namespace net
