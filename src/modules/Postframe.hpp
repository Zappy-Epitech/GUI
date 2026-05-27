#pragma once

#include "src/extern/flecs.hpp"
#include <functional>
#include <mutex>
#include <vector>

struct Later {
    static inline std::mutex mutex{};
    static inline std::vector<std::function<void(flecs::world &)>> jobs;

    Later(flecs::world &world) {
        world.module<Later>();

        world.system<>("LaterFlush")
            .kind(flecs::OnStore)
            .run([](flecs::iter &it) {
                std::vector<std::function<void(flecs::world &)>> pending;

                {
                    std::lock_guard lock(mutex);
                    pending.swap(jobs);
                }

                if (pending.empty()) {
                    return;
                }

                flecs::world world = it.world().get_world();

                world.readonly_end();

                for (auto &fn : pending) {
                    fn(world);
                }

                world.readonly_begin();
            });
    }
};

inline void later(
    std::function<void(flecs::world &)> fn) {
    std::lock_guard lock(Later::mutex);
    Later::jobs.emplace_back(std::move(fn));
}
