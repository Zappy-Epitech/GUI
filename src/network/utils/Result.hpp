#pragma once

#include "Error.hpp"

#include <expected>

namespace net {

/// Common result type for network operations.
///
/// Success contains T. Failure contains a net::Error instead of throwing.
template <typename T>
using Result = std::expected<T, Error>;

} // namespace net
