/**
 * @file Error.cpp
 * @ingroup gui_network
 * @brief Implements the error factory helpers and their message formatting.
 */
#include "Error.hpp"
#include <cstring>
#include <netdb.h>

namespace net {

/// Wraps errno into the project error type while the failing syscall context
/// is still known.
Error makeSystemError(std::string_view context) {
    return {
        .code = ErrorCode::System,
        .nativeCode = errno,
        .message = std::string(context) + ": " + std::strerror(errno)
    };
}

/// Wraps getaddrinfo() errors, which do not use errno.
Error makeDnsError(int code, std::string_view context) {
    return {
        .code = ErrorCode::Dns,
        .nativeCode = code,
        .message = std::string(context) + ": " + ::gai_strerror(code)
    };
}

/// Builds a timeout error for poll-based waits.
Error makeTimeoutError(std::string_view context) {
    return {
        .code = ErrorCode::Timeout,
        .nativeCode = 0,
        .message = std::string(context) + ": timeout"
    };
}

/// Builds a clean disconnect error when the peer closes or hangs up.
Error makeDisconnectedError(std::string_view context) {
    return {
        .code = ErrorCode::Disconnected,
        .nativeCode = 0,
        .message = std::string(context) + ": disconnected"
    };
}

/// Builds an error for API misuse, such as send() after disconnect().
Error makeInvalidStateError(std::string_view context) {
    return {
        .code = ErrorCode::InvalidState,
        .nativeCode = 0,
        .message = std::string(context)
    };
}
} // namespace net
