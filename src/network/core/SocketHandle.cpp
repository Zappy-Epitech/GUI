#include "SocketHandle.hpp"

#include <unistd.h>

namespace net {

SocketHandle::SocketHandle(int fd) noexcept
    : fd(fd) {
}

SocketHandle::~SocketHandle() {
    reset();
}

SocketHandle::SocketHandle(SocketHandle &&other) noexcept
    : fd(other.release()) {
}

SocketHandle &SocketHandle::operator=(SocketHandle &&other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

bool SocketHandle::valid() const noexcept {
    return fd >= 0;
}

int SocketHandle::get() const noexcept {
    return fd;
}

int SocketHandle::release() noexcept {
    int current = fd;
    fd = -1;
    return current;
}

void SocketHandle::reset(int next) noexcept {
    if (fd >= 0) {
        ::close(fd);
    }
    fd = next;
}

} // namespace net
