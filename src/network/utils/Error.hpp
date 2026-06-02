#pragma once

#include <string>

namespace net {

/// High-level category for errors returned by the networking layer.
enum class ErrorCode {
    System,
    Dns,
    Timeout,
    Disconnected,
    InvalidState
};

/// Error value used by Result<T>.
///
/// nativeCode stores errno for system errors or getaddrinfo()'s return code
/// for DNS errors. message is ready to display or log.
struct Error {
    ErrorCode code = ErrorCode::System;
    int nativeCode = 0;
    std::string message;
};

/// Wraps errno into the project error type while the failing syscall context
/// is still known.
Error makeSystemError(std::string_view context);

/// Wraps getaddrinfo() errors, which do not use errno.
Error makeDnsError(int code, std::string_view context);

/// Builds a timeout error for poll-based waits.
Error makeTimeoutError(std::string_view context);
/// Builds a clean disconnect error when the peer closes or hangs up.
Error makeDisconnectedError(std::string_view context);

/// Builds an error for API misuse, such as send() after disconnect().
Error makeInvalidStateError(std::string_view context);
} // namespace net
