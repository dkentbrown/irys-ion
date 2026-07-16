# Irys+Ion Player Foundation

This branch contains the first quality gate only: Standard-frame Irys movement,
authored locomotion animation, a bounded gameplay camera, one industrial movement
room, pixel-stable rendering, tuning reload, diagnostics, and deterministic tests.
It intentionally contains no combat, enemies, progression, narrative, audio, or
additional rooms.

The visual foundation uses a restrained cool-grey metal test chamber, orange-lit
traversal edges, and electric-blue identity lighting on white-haired synthetic
Irys and the Ion blade she visibly carries. The three images under
`references/player-foundation/` are committed art-direction references only and
are never loaded by the executable.

## Build

Raylib 6.0 is expected at `/opt/homebrew/opt/raylib`.

```sh
cmake -S . -B build-player-foundation \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/raylib
cmake --build build-player-foundation --parallel
ctest --test-dir build-player-foundation --output-on-failure
```

Run from the repository root:

```sh
build-player-foundation/irys_ion_foundation.app/Contents/MacOS/irys_ion_foundation
```

The runtime renders to a fixed 640x360 canvas. The canvas is nearest-neighbor
scaled to the largest integer multiple that fits the resizable window, with
letterboxing where required.

## Player Controls

| Input | Action |
|---|---|
| `A` / `D` | Move left / right |
| `W` | Jump; release early for a shorter jump |
| `S` | Drop through the one-way platform currently supporting Irys |
| `Esc` | Quit |

Facing changes immediately with horizontal intent. The authored turn pose does
not delay movement. Ordinary movement has no dash, double jump, wall action, or
combat input in this gate.

## Developer Controls

Press `F1` or backtick to enable developer mode and its overlay. The remaining developer
controls are ignored until developer mode is enabled.

| Input | Action |
|---|---|
| `F1` / backtick | Toggle developer mode and diagnostics |
| `F5` / `R` / `5` | Validate and reload `config/player_foundation.json` |
| `F6` / `Home` / `6` | Restart the room |
| `F7` / `1` | Toggle 25% slow motion |
| `F8` / `2` | Pause / resume simulation |
| `F9` / `3` | Advance one 60 Hz simulation step while paused |
| `F10` / `4` | Capture the internal canvas to `/tmp/irys-player-foundation/manual_capture.png` |
| `[` / `]` | Teleport backward / forward through named movement-test markers |

The overlay displays player collision bounds and velocity, grounded and one-way
state, coyote and jump-buffer timers, camera position and target, the camera dead
zone, room bounds, platform surfaces, interpolation alpha, animation state, and
named teleport markers. It is disabled at startup.

Malformed, missing, non-finite, or out-of-range tuning values are rejected with
an explicit error. A failed hot reload leaves the active tuning unchanged and
shows the validation error in developer mode.

## Validation Captures

Render the representative validation set without autoplay:

```sh
build-player-foundation/irys_ion_foundation.app/Contents/MacOS/irys_ion_foundation \
  --capture-dir /tmp/irys-player-foundation
```

This writes 640x360 PNGs for idle, running, braking, turning, rising, apex,
falling, landing, the orange-lit platform treatment, wide-room framing,
vertical-camera framing, and the diagnostic overlay. These captures are
validation artifacts and are not tracked.

The committed `tools/generate_foundation_assets.py` regenerates the Irys atlas,
grey metallic room layers, collision-aligned platform skins, and industrial
material atlas using only the Python standard library. Python is not used by
configuration, compilation, testing, or play.
