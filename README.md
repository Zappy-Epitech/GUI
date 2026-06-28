# Zappy GUI

Zappy GUI is a C++23 graphical client for the Zappy server. It connects to a running Zappy server, reads the official GUI protocol, and renders the game world with raylib, flecs, and a Minecraft-inspired visual style.

## Features

- 3D map rendering with resources, eggs, players, teams, and incantation effects.
- Minecraft-style player skins loaded from `assets/skins/`.
- Player animations for movement, expulsion, egg laying, and level-up/incantation events.
- Team labels and player levels above players.
- Broadcast message bubbles above the speaking player.
- Team panels with player lists.
- Player inventory modal with resource counts and a 3D skin preview.
- Runtime server frequency control through the GUI.
- Configurable settings for volume, render distance, team labels, hover highlight, and log panel visibility.
- Render-distance filtering for large maps.

## Requirements

The project expects:

- `clang++`
- `clang`
- `make`
- `pkg-config`
- raylib development files
- Criterion, only needed for `make test`

The GUI embeds some third-party headers and sources in `src/extern/`, including raygui and flecs.

## Build

```sh
make
```

or:

```sh
make build
```

The executable is generated at:

```sh
bin/app
```

For an optimized build:

```sh
make perf
```

The optimized executable is generated at:

```sh
bin/app_perf
```

To clean build files:

```sh
make clean
make fclean
make re
```

## Run

Start the GUI with:

```sh
make run
```

or directly:

```sh
./bin/app
```

On the home screen, enter the server host and port, then press `Play`.

Default values:

- Host: `127.0.0.1`
- Port: `4242`

The Zappy server must already be running and must accept GUI clients.

## Controls

Camera movement:

- `W`: move forward
- `S`: move backward
- `A`: move left
- `D`: move right
- `Q`: move down
- `E`: move up
- Arrow keys: rotate/look around

UI:

- Click a team button to open its player list.
- Click a player in the list to open the inventory modal.
- Press `Escape` or click `x` to close modals.
- Use the `Freq` slider in game to request a new server frequency.
- Use `Settings` to configure volume, render distance, labels, hover highlight, and log panel visibility.

## Skins

Player skins are loaded from:

```sh
assets/skins/
```

Add Minecraft skin PNG files to that folder to make them available to the GUI. The renderer supports the normal Minecraft skin layout and the second overlay layer for head, body, arms, and legs.

## Tests

Run tests with:

```sh
make test
```

Tests use Criterion. If Criterion is not installed, the test build will fail even if the application itself builds correctly.

## Project Layout

- `src/core/`: application primitives, raylib setup, settings, GUI helpers, spatial components.
- `src/scenes/`: home screen, game UI, settings modal, end-game screen.
- `src/gameplay/`: map, players, teams, eggs, movement, assets, simulation state.
- `src/minecraft/`: Minecraft-style rendering and animation.
- `src/network/`: TCP client, session handling, network ECS integration.
- `src/protocol/`: Zappy protocol parsing and command application.
- `src/parsing/`: scanner utilities.
- `src/extern/`: embedded third-party dependencies.
- `assets/`: models, textures, skins, sounds.
- `tests/`: Criterion test suites.
