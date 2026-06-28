/**
 * @file Result.hpp
 * @ingroup gui_network
 * @brief Alias of std::expected<T, Error> used as the network result type.
 */
#pragma once

#include "Error.hpp"

#include <expected>

namespace net {

/** @brief Common result type for network operations.
 *
 * Success contains T. Failure contains a net::Error instead of throwing.
 * @ingroup gui_network
 */
template <typename T>
using Result = std::expected<T, Error>;

} // namespace net
