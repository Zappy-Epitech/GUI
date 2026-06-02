#include "SocketPoll.hpp"

#include <cerrno>
#include <limits>
#include <poll.h>
#include <sys/socket.h>

namespace net {
namespace {

int toPollTimeout(std::chrono::milliseconds timeout) {
    if (timeout.count() < 0) {
        return -1;
    }

    if (timeout.count() > std::numeric_limits<int>::max()) {
        return std::numeric_limits<int>::max();
    }

    return static_cast<int>(timeout.count());
}

Result<void> waitFor(int fd, short events, std::chrono::milliseconds timeout, std::string_view context) {
    pollfd item{
        .fd = fd,
        .events = events,
        .revents = 0
    };

    while (true) {
        const int result = ::poll(&item, 1, toPollTimeout(timeout));

        if (result > 0) {
            if (item.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                if (auto checked = checkSocketError(fd, context); !checked) {
                    return checked;
                }
                return std::unexpected(makeDisconnectedError(context));
            }
            return {};
        }

        if (result == 0) {
            return std::unexpected(makeTimeoutError(context));
        }

        if (errno != EINTR) {
            return std::unexpected(makeSystemError(context));
        }
    }
}

} // namespace

Result<void> checkSocketError(int fd, std::string_view context) {
    int socketError = 0;
    socklen_t length = sizeof(socketError);

    if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &socketError, &length) < 0) {
        return std::unexpected(makeSystemError("getsockopt(SO_ERROR)"));
    }

    if (socketError != 0) {
        errno = socketError;
        return std::unexpected(makeSystemError(context));
    }

    return {};
}

Result<void> waitReadable(int fd, std::chrono::milliseconds timeout, std::string_view context) {
    return waitFor(fd, POLLIN, timeout, context);
}

Result<void> waitWritable(int fd, std::chrono::milliseconds timeout, std::string_view context) {
    return waitFor(fd, POLLOUT, timeout, context);
}

} // namespace net
