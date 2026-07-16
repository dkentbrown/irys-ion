# Irys+Ion

Irys+Ion is a native C++20/Raylib cyberpunk action platformer. Ion, a persistent blade intelligence, selects a generated synthetic Irys substrate and fights through a fixed five-boss campaign. Ordinary movement and the directional combat Language—Advance, Ascend, Descend, and Prime—form one positional system. Bodies, damage, and post-boss frame decisions vary; the route and canonical difficulty do not.

The `sol/one-shot-implementation` branch contains the complete first-pass game: Ark selection, four anatomy configurations, nine ordinary-enemy forms, authored rooms, five bosses, three optional frame decisions, false finale, shutdown return, final Ark duel, ending, synthesized music/audio, persistence, and developer validation tools.

## Prerequisites

- macOS (the verified target)
- Apple Clang with C++20 support
- CMake 3.24 or newer
- Raylib 6.0

Windows and Linux portability is preserved in the code, but those platforms have not been verified. No Homebrew package installation is performed by the build. When Raylib is installed under Homebrew, its prefix is normally `/opt/homebrew/opt/raylib`.

## Configure and build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/raylib
cmake --build build --parallel
```

The executable is:

```text
build/irys-ion
```

Run it from the repository root:

```sh
./build/irys-ion
```

If CMake cannot find Raylib, set `CMAKE_PREFIX_PATH` to the directory containing `lib/cmake/raylib`. The build includes a pinned Raylib 6.0 `FetchContent` fallback; disable network fallback with `-DIRYSION_FETCH_RAYLIB=OFF` to require an installed package.

## Controls

Gameplay:

- `A` / `D`: ordinary movement and immediate facing
- `W`: jump; release early for a shorter jump
- `S`: drop through one-way platforms
- horizontal arrow matching facing: Advance
- horizontal arrow opposite facing: arm Prime
- `Up Arrow`: Ascend
- `Down Arrow`: Descend
- `Escape`: pause or back
- `Enter` / `Space`: confirm

Normal Techniques redirect immediately and are never buffered. Primed Techniques are committed. Prime is the only retained combat intention.

Menus use arrow keys, Enter/Space, Escape, and Backspace as shown on screen. Settings persist and change presentation only; there are no alternate difficulty modes.

## Save/profile behavior

There is one versioned JSON profile and no active-run save. Quitting abandons the current attempt. The profile stores settings, aggregate statistics, five recent runs, and the completion flag.

Default locations:

- macOS/Linux: `~/.local/share/irys-ion/profile.json`
- Windows portability path: `%APPDATA%/IrysIon/profile.json`

Writes use a temporary file and preserve a previous-valid `.bak` file. A malformed main profile recovers from the backup; if neither is valid the game safely starts a default profile. To reset manually, remove `profile.json` and `profile.json.bak` while the game is closed. Developer mode also exposes an explicit profile reset (`F12`).

## Tests and validation

```sh
ctest --test-dir build --output-on-failure
./build/irys-ion --validate-campaign
```

The deterministic validation checks tuning, PRNG repeatability, constrained body generation, all anatomies, Prime and combat authority, hit eligibility, profile recovery, frame replacement, all nine enemy forms, all five bosses, and the complete campaign-state chain.

Actual executable smoke run:

```sh
./build/irys-ion --smoke-frames 180
```

Accelerated complete-campaign validation with visual checkpoint export:

```sh
./build/irys-ion --autoplay --smoke-frames 3000 --capture-dir /tmp/irys-ion-captures
```

`--autoplay` is a developer-only deterministic traversal. It does not alter standard play or unlock progression.

The executable failure/Ark-return path can be exercised separately with `--autoplay-failure` and the same smoke/capture arguments.

## Developer mode

Start normally with developer diagnostics enabled:

```sh
./build/irys-ion --developer
```

Direct authored targets:

```sh
./build/irys-ion --developer --boss 5
./build/irys-ion --developer --room 3 2
```

Boss numbers are 1–5; stage and room numbers are 1-based.

Developer keys:

- `F1`: hitbox, hurtbox, attack-volume, authority, frame, seed, and state overlay
- `F2`: testing invulnerability
- `F3`: restart current room or encounter
- `F4`: restart current boss
- `F5`: reload and validate tuning JSON
- `F6`: clear the current encounter
- `F7`: restore current frame integrity
- `F8`: advance a cleared room
- `F9`: jump to the current campaign boss
- `F10`: single fixed-step while paused
- `F12`: reset profile/statistics

Developer invulnerability is never enabled in standard play and does not change the canonical no-invulnerability-frame rules.

## Art and audio

The first-pass art is rendered as code-native modular pixel construction at a 640×360 internal resolution. It includes the four Irys anatomy rigs, fixed Ion and Anti-Irys designs, inherited enemy silhouettes, five distinct bosses, region palettes, combat effects, Ark pods, inspection interfaces, final arena transformations, and ending presentation. Nearest-neighbor output is used, with integer scaling available in settings.

Audio is synthesized at startup through Raylib from authored wave definitions. The runtime contains separate Technique identities, Prime cues, hits, damage, interface/dialogue signals, Ark and region beds, dedicated boss harmonies, shutdown treatment, and a fuller multi-harmonic orchestral treatment for Anti-Irys. No implementation-time asset tool or external audio middleware is needed.

## Dependencies

- Raylib 6.0, resolved from an installed CMake package or the pinned 6.0 fallback
- nlohmann/json 3.11.3, vendored as the official single header under `third_party/nlohmann`
- C++ standard library

The normal build never downloads JSON. Vendored source and MIT provenance are recorded in `third_party/nlohmann/README.md` and `third_party/nlohmann/LICENSE.MIT`.
