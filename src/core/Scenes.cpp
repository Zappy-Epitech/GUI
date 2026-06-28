/**
 * @file Scenes.cpp
 * @ingroup gui_core
 * @brief Implements scene lifecycle: despawn-on-enter/exit, InScene sync and enterScene helpers.
 */
#include "Scenes.hpp"
#include "src/core/Core.hpp"
#include "src/extern/flecs.h"

namespace {

void despawnMatching(flecs::world world, flecs::entity_t relation, flecs::entity_t scene) {
    auto query = world.query_builder()
        .query_flags(EcsQueryMatchDisabled)
        .with(relation, scene)
        .build();

    world.defer([&query] {
        query.each([](flecs::entity entity) {
            entity.destruct();
        });
    });
}

void setEntityEnabled(flecs::entity entity, bool enabled) {
    if (enabled) {
        entity.enable();
    } else {
        entity.disable();
    }
}

void setInSceneEnabled(flecs::world world, flecs::entity_t scene, bool enabled) {
    auto query = world.query_builder()
        .query_flags(EcsQueryMatchDisabled)
        .with<InScene>(scene)
        .build();

    world.defer([&query, enabled] {
        query.each([enabled](flecs::entity entity) {
            setEntityEnabled(entity, enabled);
        });
    });
}

void syncInScene(flecs::world world, flecs::entity entity) {
    const CurrentScene *current = world.try_get<CurrentScene>();

    const bool enabled = current && entity.has<InScene>(current->id);

    world.defer([entity, enabled] {
        setEntityEnabled(entity, enabled);
    });
}

} // namespace

/// Registers generic scene lifecycle components and systems.
Scenes::Scenes(flecs::world &world) {
    world.module<Scenes>("scenes").child_of<Core>();
    world.component<DespawnOnEnter>();
    world.component<DespawnOnExit>();
    world.component<InScene>();

    world.observer("SyncInScene")
        .event(flecs::OnAdd)
        .with<InScene>(flecs::Wildcard)
        .each([](flecs::entity e) {
            syncInScene(e.world(), e);
        });

    world.component<CurrentScene>()
        .on_replace([world](const CurrentScene &current, const CurrentScene &next) {
            if (current.id != next.id) {
                despawnMatching(world, world.entity<DespawnOnExit>().id(), current.id);
                setInSceneEnabled(world, current.id, false);
            }
        })
        .on_set([](flecs::entity e, const CurrentScene &scene) {
            flecs::world world = e.world();
            despawnMatching(world, world.entity<DespawnOnEnter>().id(), scene.id);
            setInSceneEnabled(world, scene.id, true);
        });
}

void enterScene(flecs::world &world, flecs::entity_t scene) {
    if (const CurrentScene *current = world.try_get<CurrentScene>()) {
        if (current->id == scene) {
            return;
        }
    }

    world.set<CurrentScene>({ .id = scene });
}

bool isCurrentScene(const flecs::world &world, flecs::entity_t scene) {
    if (const CurrentScene *current = world.try_get<CurrentScene>()) {
        return current->id == scene;
    }
    return false;
}
