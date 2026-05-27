#include "Home.hpp"
#include "../../extern/flecs.hpp"
#include "src/modules/Gui.hpp"
#include "src/modules/Spatial.hpp"
#include "src/modules/scenes/Game.hpp"

Home::Home(flecs::world &world) {
    world.entity("Play Button")
        .set(Button("Play !"))
        .set(Position2::center())
        .set(OnClick([world](flecs::entity) mutable {
            world.entity<Home>().destruct();
            world.import<Game>();
        }));

    world.entity("Ip Input")
        .set(TextInput("Enter IP"))
        .set(Position2::center().sub_y(100))
        .set(OnTextUpdate([](flecs::entity, std::string &update) {
            printf("Text updated: %s\n", update.c_str());
        }));
}
