#pragma once

#include "src/extern/flecs.h"

#include <format>
#include <utility>

struct DespawnOnEnter {};
struct DespawnOnExit {};
struct InScene {};

template <typename T>
struct SceneTag {};

struct CurrentScene {
    flecs::entity_t id = 0;
};

void enterScene(flecs::world &world, flecs::entity_t scene);
bool isCurrentScene(const flecs::world &world, flecs::entity_t scene);

template <typename T>
flecs::entity_t sceneId(const flecs::world &world) {
    return world.entity<SceneTag<T>>(std::format("Scene({})", flecs::_::type_name<T>()).c_str()).id();
}

template <typename T>
void enterScene(flecs::world &world) {
    enterScene(world, sceneId<T>(world));
}

template <typename T>
bool isCurrentScene(const flecs::world &world) {
    return isCurrentScene(world, sceneId<T>(world));
}

template <typename T, typename Fn>
void onEnterScene(flecs::world &world, const char *name, Fn &&fn) {
    world.observer<const CurrentScene>(name)
        .event(flecs::OnSet)
        .each([callback = std::forward<Fn>(fn)](flecs::entity e, const CurrentScene &) {
            flecs::world world = e.world();

            if (isCurrentScene<T>(world)) {
                callback(world);
            }
        });
}

template <typename T, typename Fn>
void onExitScene(flecs::world &world, const char *name, Fn &&fn) {
    world.observer<const CurrentScene>(name)
        .event(flecs::OnSet)
        .each([callback = std::forward<Fn>(fn)](flecs::entity e, const CurrentScene &) {
            flecs::world world = e.world();

            if (!isCurrentScene<T>(world)) {
                callback(world);
            }
        });
}

/// Registers generic scene lifecycle components and systems.
struct Scenes {
    /// Imports scene lifecycle infrastructure.
    Scenes(flecs::world &world);
};
