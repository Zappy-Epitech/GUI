/**
 * @file Error.hpp
 * @ingroup gui_network
 * @brief Error type, error categories, and factory helpers for the network layer.
 */
#pragma once

#include <string>

namespace net {

/// High-level category for errors returned by the networking layer.
enum class ErrorCode {
    /// Error reported by a POSIX syscall.
    System,
    /// Error reported by getaddrinfo().
    Dns,
    /// Operation exceeded its configured timeout.
    Timeout,
    /// Peer closed or invalidated the connection.
    Disconnected,
    /// API was used in an invalid state.
    InvalidState
};

/** @brief Error value used by Result<T>.
 *
 * nativeCode stores errno for system errors or getaddrinfo()'s return code
 * for DNS errors. message is ready to display or log.
 * @ingroup gui_network
 */
struct Error {
    ErrorCode code = ErrorCode::System; ///< High-level error category.
    int nativeCode = 0;                 ///< errno (System) or getaddrinfo() code (Dns); 0 otherwise.
    std::string message;                ///< Display-ready error message, prefixed with the failing context.
};

/** @brief Wraps the current errno into the project error type.
 * @param context Short label of the failing syscall, prepended to the message.
 * @return Error with code System, nativeCode set to errno, and message "context: strerror(errno)".
 */
Error makeSystemError(std::string_view context);

/** @brief Wraps a getaddrinfo() failure, which does not use errno.
 * @param code The non-zero return code from getaddrinfo().
 * @param context Short label of the resolution step, prepended to the message.
 * @return Error with code Dns, nativeCode set to @p code, and message "context: gai_strerror(code)".
 */
Error makeDnsError(int code, std::string_view context);

/** @brief Builds a timeout error for poll-based waits.
 * @param context Short label of the operation that timed out, prepended to the message.
 * @return Error with code Timeout, nativeCode 0, and message "context: timeout".
 */
Error makeTimeoutError(std::string_view context);

/** @brief Builds a clean disconnect error when the peer closes or hangs up.
 * @param context Short label of the operation that observed the close, prepended to the message.
 * @return Error with code Disconnected, nativeCode 0, and message "context: disconnected".
 */
Error makeDisconnectedError(std::string_view context);

/** @brief Builds an error for API misuse, such as send() after disconnect().
 * @param context Description of the misuse, used verbatim as the message.
 * @return Error with code InvalidState, nativeCode 0, and message set to @p context.
 */
Error makeInvalidStateError(std::string_view context);
} // namespace net
