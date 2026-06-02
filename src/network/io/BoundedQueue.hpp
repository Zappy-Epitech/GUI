#pragma once

#include <cstddef>
#include <deque>
#include <utility>

namespace net {

/// FIFO queue with a fixed maximum size. When full, oldest values are dropped.
template <typename T, std::size_t MaxSize>
class BoundedQueue {
  public:
    void push(T value) {
        if (values.size() >= MaxSize) {
            values.pop_front();
        }
        values.push_back(std::move(value));
    }

    bool pop(T &out) {
        if (values.empty()) {
            return false;
        }
        out = std::move(values.front());
        values.pop_front();
        return true;
    }

    void clear() {
        values.clear();
    }

  private:
    std::deque<T> values;
};

} // namespace net
