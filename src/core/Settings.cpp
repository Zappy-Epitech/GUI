#include "Settings.hpp"
#include "src/core/Core.hpp"
#include "src/extern/flecs.h"

#include <raylib.h>

/// Registers GUI settings state and systems.
Settings::Settings(flecs::world &world) {
    world.module<Settings>("settings").child_of<Core>();

    world.component<GuiSettings>()
        .member(flecs::F32, "masterVolume")
        .member(flecs::Bool, "showTeamNames")
        .member(flecs::Bool, "highlightTeamOnHover");

    world.singleton<GuiSettings>().set<GuiSettings>({});

    world.system<const GuiSettings>("ApplyMasterVolume")
        .kind(flecs::PostUpdate)
        .each([](const GuiSettings &settings) {
            if (IsAudioDeviceReady()) {
                SetMasterVolume(settings.masterVolume);
            }
        });
}
