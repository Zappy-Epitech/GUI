#include "AdressList.hpp"
#include <string>

AddressList::AddressList(std::string_view host, std::uint16_t port) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const std::string hostString(host);
    const std::string portString = std::to_string(port);

    this->error = ::getaddrinfo(hostString.c_str(), portString.c_str(), &hints, &this->head);
}

AddressList::~AddressList() {
    if (this->head != nullptr) {
        ::freeaddrinfo(this->head);
    }
}

[[nodiscard]] int AddressList::getError() const noexcept {
    return this->error;
}

[[nodiscard]] addrinfo *AddressList::begin() const noexcept {
    return this->head;
}
