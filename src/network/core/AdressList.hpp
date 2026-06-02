#pragma once

#include <cstdint>
#include <netdb.h>
#include <string_view>

/// RAII wrapper for getaddrinfo() results.
class AddressList {
  public:
    AddressList(std::string_view host, std::uint16_t port);
    ~AddressList();

    AddressList(const AddressList &) = delete;
    AddressList &operator=(const AddressList &) = delete;

    [[nodiscard]] int getError() const noexcept;

    [[nodiscard]] addrinfo *begin() const noexcept;

  private:
    addrinfo *head = nullptr;
    int error = 0;
};
