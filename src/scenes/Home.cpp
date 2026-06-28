#include "Home.hpp"
#include "src/core/Core.hpp"
#include "src/core/Gui.hpp"
#include "src/core/Scenes.hpp"
#include "src/core/Spatial.hpp"
#include "src/extern/flecs.h"
#include "src/network/NetworkModule.hpp"
#include "src/scenes/AppScenes.hpp"
#include "src/scenes/Game.hpp"
#include "src/scenes/Settings.hpp"

#include <charconv>
#include <raylib.h>
#include <string>

struct HomeConnectForm {
    std::string host = "127.0.0.1";
    std::string port = "4242";
};

bool parsePort(const std::string &text, std::uint16_t &port) {
    unsigned value = 0;
    const char *begin = text.data();
    const char *end = text.data() + text.size();
    auto [ptr, error] = std::from_chars(begin, end, value);

    if (error != std::errc() || ptr != end || value == 0 || value > 65535) {
        return false;
    }

    port = static_cast<std::uint16_t>(value);
    return true;
}

/// Registers the home scene.
Home::Home(flecs::world &world) {
    auto module = world.module<Home>("home").child_of<AppScenes>();
    world.singleton<HomeConnectForm>().set<HomeConnectForm>({}).child_of(module);

    onEnterScene<Home>(world, "EnterHome", [](flecs::world &world) {
        world.entity("Ip Input")
            .set(TextInput(world.get<HomeConnectForm>().host.c_str()))
            .set(Position2::center().sub_y(100))
            .set(OnTextUpdate([](flecs::entity e, std::string &update) {
                e.world().get_mut<HomeConnectForm>().host = update;
            }))
            .add<DespawnOnExit>(sceneId<Home>(world));

        world.entity("Port Input")
            .set(TextInput(world.get<HomeConnectForm>().port.c_str()))
            .set(Position2::center().sub_y(50))
            .set(OnTextUpdate([](flecs::entity e, std::string &update) {
                e.world().get_mut<HomeConnectForm>().port = update;
            }))
            .add<DespawnOnExit>(sceneId<Home>(world));

        world.entity("Play Button")
            .set(Button("Play !"))
            .set(Position2::center().add_y(10))
            .set(OnClick([](flecs::entity e) {
                flecs::world world = e.world();
                auto &form = world.get_mut<HomeConnectForm>();
                std::uint16_t port = 0;

                if (!parsePort(form.port, port)) {
                    world.entity()
                        .set(ScreenMessage("Invalid port"))
                        .set(Color(RED))
                        .set(Lifetime(2));
                    return;
                }

                connectToServer(world, NetworkConfig(form.host.c_str(), port));
                enterScene<Game>(world);
            }))
            .add<DespawnOnExit>(sceneId<Home>(world));

        world.entity("Settings Button")
            .set(Button("Settings"))
            .set(Position2::center().add_y(90))
            .set(OnClick([](flecs::entity e) {
                flecs::world world = e.world();
                openSettingsModal(world);
            }))
            .add<DespawnOnExit>(sceneId<Home>(world));
    });
}
