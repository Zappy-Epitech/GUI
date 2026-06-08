#pragma once

#include <cstdint>
#include <netdb.h>
#include <string_view>

/// RAII wrapper for getaddrinfo() results.
class AddressList {
  public:
    /// Resolves host and port with getaddrinfo().
    AddressList(std::string_view host, std::uint16_t port);
    /// Frees the getaddrinfo() linked list.
    ~AddressList();

    /// Prevents copying owned addrinfo storage.
    AddressList(const AddressList &) = delete;
    /// Prevents assigning owned addrinfo storage.
    AddressList &operator=(const AddressList &) = delete;

    /// Returns the getaddrinfo() error code.
    [[nodiscard]] int getError() const noexcept;

    /// Returns the first resolved address.
    [[nodiscard]] addrinfo *begin() const noexcept;

  private:
    /// Head of the getaddrinfo() result list.
    addrinfo *head = nullptr;
    /// getaddrinfo() result code.
    int error = 0;
};
