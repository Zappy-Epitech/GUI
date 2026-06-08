#pragma once

namespace net {

/// RAII owner for a POSIX socket descriptor.
class SocketHandle {
  public:
    /// Creates an empty socket handle.
    SocketHandle() = default;
    /// Takes ownership of an existing descriptor.
    explicit SocketHandle(int fd) noexcept;
    /// Closes the owned descriptor.
    ~SocketHandle();

    /// Prevents copying a unique descriptor owner.
    SocketHandle(const SocketHandle &) = delete;
    /// Prevents assigning a unique descriptor owner.
    SocketHandle &operator=(const SocketHandle &) = delete;

    /// Moves descriptor ownership from another handle.
    SocketHandle(SocketHandle &&other) noexcept;
    /// Replaces this descriptor with another handle's descriptor.
    SocketHandle &operator=(SocketHandle &&other) noexcept;

    /// Returns true when a descriptor is owned.
    [[nodiscard]] bool valid() const noexcept;
    /// Returns the owned descriptor or -1.
    [[nodiscard]] int get() const noexcept;

    /// Releases ownership without closing.
    int release() noexcept;
    /// Closes the current descriptor and optionally stores a new one.
    void reset(int next = -1) noexcept;

  private:
    /// Owned POSIX file descriptor.
    int fd = -1;
};

} // namespace net
