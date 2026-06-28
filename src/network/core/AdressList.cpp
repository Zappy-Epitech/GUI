/**
 * @file AdressList.cpp
 * @ingroup gui_network
 * @brief Implements getaddrinfo()-based host/port resolution and cleanup.
 */
#include "AdressList.hpp"
#include <string>

/// Resolves host and port with getaddrinfo().
AddressList::AddressList(std::string_view host, std::uint16_t port) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const std::string hostString(host);
    const std::string portString = std::to_string(port);

    this->error = ::getaddrinfo(hostString.c_str(), portString.c_str(), &hints, &this->head);
}

/// Frees the getaddrinfo() linked list.
AddressList::~AddressList() {
    if (this->head != nullptr) {
        ::freeaddrinfo(this->head);
    }
}

/// Returns the getaddrinfo() error code.
[[nodiscard]] int AddressList::getError() const noexcept {
    return this->error;
}

/// Returns the first resolved address.
[[nodiscard]] addrinfo *AddressList::begin() const noexcept {
    return this->head;
}
