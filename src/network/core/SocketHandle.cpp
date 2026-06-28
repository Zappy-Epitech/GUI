/**
 * @file SocketHandle.cpp
 * @ingroup gui_network
 * @brief Implements RAII move semantics and close() for the socket descriptor.
 */
#include "SocketHandle.hpp"

#include <unistd.h>

namespace net {

/// Takes ownership of an existing descriptor.
SocketHandle::SocketHandle(int fd) noexcept
    : fd(fd) {
}

/// Closes the owned descriptor.
SocketHandle::~SocketHandle() {
    reset();
}

/// Moves descriptor ownership from another handle.
SocketHandle::SocketHandle(SocketHandle &&other) noexcept
    : fd(other.release()) {
}

/// Replaces this descriptor with another handle's descriptor.
SocketHandle &SocketHandle::operator=(SocketHandle &&other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

/// Returns true when a descriptor is owned.
bool SocketHandle::valid() const noexcept {
    return fd >= 0;
}

/// Returns the owned descriptor or -1.
int SocketHandle::get() const noexcept {
    return fd;
}

/// Releases ownership without closing.
int SocketHandle::release() noexcept {
    int current = fd;
    fd = -1;
    return current;
}

/// Closes the current descriptor and optionally stores a new one.
void SocketHandle::reset(int next) noexcept {
    if (fd >= 0) {
        ::close(fd);
    }
    fd = next;
}

} // namespace net
