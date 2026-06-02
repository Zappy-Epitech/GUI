#pragma once

namespace net {

/// RAII owner for a POSIX socket descriptor.
class SocketHandle {
  public:
    SocketHandle() = default;
    explicit SocketHandle(int fd) noexcept;
    ~SocketHandle();

    SocketHandle(const SocketHandle &) = delete;
    SocketHandle &operator=(const SocketHandle &) = delete;

    SocketHandle(SocketHandle &&other) noexcept;
    SocketHandle &operator=(SocketHandle &&other) noexcept;

    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int get() const noexcept;

    int release() noexcept;
    void reset(int next = -1) noexcept;

  private:
    int fd = -1;
};

} // namespace net
