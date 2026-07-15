# Irys+Ion — Technical Specification

**Version:** 1.0
**Status:** Approved
**Last Updated:** 2026-07-15
**Owner:** Technical Direction

---

# Purpose

This document defines the technical architecture and implementation constraints for Irys+Ion.

It translates the approved design into the smallest complete C++ and Raylib architecture capable of implementing the full playable game.

The first Sol implementation must attempt the complete game, not a vertical slice.

This document does not authorize new creative systems.

---

# Implementation Objective

The first Sol implementation must attempt the complete playable game.

It must not stop at a vertical slice.

It must include:

- title and menu flow;
- Ark;
- candidate selection;
- generated Irys substrates;
- full ordinary movement;
- the complete Language;
- all outward stages;
- all three enemy lineages and their visible generations;
- all five bosses;
- post-boss substrate selection after Bosses 1–3;
- control-center transfer sequence;
- shutdown return;
- final Ark duel;
- ending;
- run failure and return;
- coherent first-pass pixel art;
- coherent first-pass music and sound;
- dialogue;
- HUD;
- settings;
- lifetime statistics;
- developer tools.

The first implementation may later be tuned and polished, but it must not leave complete stages, bosses, systems, visual identities, or audio identities as placeholders.

Major characters must not be represented as final colored rectangles or generic primitives.

---

# Technology

Use:

- C++20;
- Raylib;
- CMake;
- standard-library facilities;
- one pinned vendored copy of `nlohmann/json`.

Requirements:

- The normal build must not download JSON.
- Record the vendored JSON version.
- Raylib may use an installed package with a pinned CMake FetchContent fallback.
- Do not track an unbounded Raylib branch.
- Produce one native executable.
- No external engine.
- No ECS framework.
- No physics middleware.
- No scripting runtime.
- No audio middleware.
- No runtime package manager.

---

# Platform

Initial required and verified target:

- macOS.

Eventual required ports:

- Windows;
- Linux.

Therefore:

- use portable C++20;
- use Raylib abstractions;
- use `std::filesystem`;
- avoid Apple-specific gameplay, rendering, input, timing, audio, and persistence code;
- isolate unavoidable platform-specific behavior behind narrow interfaces.

The first Sol session does not need to verify Windows or Linux.

---

# Build

Provide a conventional CMake build.

Expected workflow should be equivalent to:

```sh
cmake -S . -B build
cmake --build build
```

Report the executable path.

CMake must:

- require C++20;
- resolve Raylib predictably;
- include vendored JSON;
- expose or copy runtime assets;
- support development and release builds;
- produce one executable.

Do not create a complicated engine-style build graph.

---

# Source Architecture

Use explicit gameplay types, not a generalized ECS.

Expected major types include:

- Application;
- Game;
- Profile;
- Settings;
- RunState;
- CampaignState;
- Room;
- Player;
- Enemy;
- Boss;
- Projectile;
- Hazard;
- Platform;
- CameraController;
- AudioDirector;
- DialogueController;
- DeveloperTools.

Use small shared structs for:

- transform;
- velocity;
- health;
- hitboxes;
- hurtboxes;
- attack volumes;
- animation state;
- sprite parts;
- damage;
- timing;
- rendering;
- audio cues.

Prefer explicit composition and authored state machines.

Do not build a generalized entity framework or deep inheritance tree.

Balance and tuning data must remain separable from behavior code.

---

# Data and Tuning

Use a hybrid model.

Keep structural behavior in C++:

- state machines;
- boss phases;
- stage and room topology;
- narrative flow;
- Technique logic;
- encounter composition;
- authored attack behavior.

Use external JSON for frequently tuned values such as:

- health;
- damage;
- movement speed;
- acceleration;
- jump values;
- Technique distances;
- Technique timing;
- telegraph duration;
- recovery;
- body-generation ranges and weights;
- critical-hit ranges;
- camera values;
- effects;
- audio levels;
- accessibility values.

Requirements:

- central configuration layer;
- no raw JSON access scattered through gameplay code;
- validation;
- safe compiled defaults;
- useful error reporting;
- developer hot reload where practical;
- malformed tuning data must not crash the game when safe fallback is possible.

---

# Runtime States

Use explicit high-level states including:

1. Boot.
2. Main Menu.
3. Ark.
4. Ark Candidate Selection.
5. Stage Room.
6. Boss Encounter.
7. Post-Boss Substrate Selection.
8. Area Transition.
9. Control-Center Transfer.
10. Shutdown Return.
11. Final Ark Duel.
12. Run Failure.
13. Ending.
14. Pause.
15. Settings.

Dialogue occurs only in safe non-combat states.

Do not place dialogue boxes over live enemy encounters.

Ark dialogue, candidate selection, post-boss selection, biome transitions, boss aftermath, control-center reveal, and ending scenes do not need to pause combat because active combat simulation is not running in those states.

---

# Run Structure

Every run follows the fixed campaign:

1. Ark candidate selection.
2. First outward region.
3. Boss 1.
4. Post-Boss Selection 1.
5. Second outward region.
6. Boss 2.
7. Post-Boss Selection 2.
8. Third outward region.
9. Boss 3.
10. Post-Boss Selection 3.
11. Control-center region.
12. Boss 4 command mech.
13. Anti-Irys transfer.
14. Shutdown return.
15. Final Ark duel.
16. Ending.

Target skilled successful-run duration:

- approximately 35–45 minutes.

Failed runs usually end sooner.

The first implementation must include the complete campaign.

---

# Run Persistence

Runs are one-shot attempts.

Closing the application or quitting to the menu abandons the run.

Do not implement:

- active-run saves;
- suspend saves;
- continue tokens;
- checkpoints;
- mid-run autosave continuation;
- a Continue menu option.

Pause is allowed during the current session.

---

# Profile and Save Data

Use one local profile only.

Store:

- settings;
- aggregate lifetime statistics;
- five most recent detailed run records;
- `hasClearedGame`.

Do not store active-run state.

The completion flag reserves a future hook for post-clear secrets but does not require secret content in this implementation.

Suggested aggregate statistics include:

- attempts;
- successful runs;
- completion rate;
- total playtime;
- fastest successful run;
- boss defeats;
- deaths by boss or stage;
- furthest stage reached;
- starting-frame selections;
- post-boss swaps accepted;
- post-boss swaps declined;
- runs completed without swapping;
- total substrates lost.

Each of the five recent-run records may store:

- internal run seed;
- starting frame summary;
- swap decisions;
- bosses reached;
- result;
- duration.

Display these statistics on an Ark diagnostics panel.

Persistence format:

- versioned JSON;
- temp-file write;
- atomic replacement where practical;
- one previous-valid backup where easy to implement;
- backup recovery if the main file is malformed;
- default reset with a brief warning if both fail;
- migration hooks for future schema versions.

No multiple profiles or save slots.

---

# Internal Randomness

Use an internal deterministic PRNG.

The run seed affects:

- Ark candidate generation;
- post-boss substrate generation;
- bounded attack selection where reproducibility is useful.

Maps, rooms, bosses, dialogue, art, music, and encounter definitions remain authored and fixed.

Do not expose:

- player-facing seed entry;
- daily seeds;
- challenge seeds;
- replay-by-seed menus.

The seed may appear in developer tools and logs.

---

# Timing

Gameplay simulation runs at fixed 60 Hz.

The fixed update owns:

- movement;
- collision;
- combat authority;
- hit registration;
- enemy AI;
- boss AI;
- timers;
- campaign state;
- active gameplay animation state.

Rendering may run independently.

Use interpolation where useful.

Interpolation must not affect simulation decisions or collision.

Limit catch-up steps.

Discard excessive accumulated time rather than processing an unlimited update spiral.

Pause stops simulation time completely.

---

# Input

Initial input support is keyboard only.

Canonical controls:

- A: move left;
- D: move right;
- W: jump;
- S: drop through one-way platforms;
- horizontal arrow matching facing: Advance;
- horizontal arrow opposite facing: Prime;
- Up Arrow: Ascend;
- Down Arrow: Descend;
- Escape: pause/back;
- Enter or Space: confirm.

Gameplay code consumes abstract actions, not raw key codes.

Controller support is deferred but must be addable through the action layer.

Full key rebinding is deferred.

No controller UI, vibration, or controller testing is required initially.

---

# Ordinary Movement

Implement:

- horizontal acceleration;
- horizontal deceleration;
- jumping;
- gravity;
- air control;
- moving-platform support;
- one-way platforms;
- S drop-through;
- short tunable coyote time;
- short tunable ordinary-jump buffer;
- variable jump height when W is released early.

Ordinary-jump buffering does not create a Technique buffer.

Do not implement:

- double jump;
- wall jump;
- wall slide;
- dash;
- air dash;
- teleport;
- grappling.

---

# Facing

When ordinary movement has authority:

- A immediately changes facing left;
- D immediately changes facing right;
- facing may change immediately in the air.

There is no mechanical turn lock.

Animation must remain visually smooth through mirroring, blending, or a brief authored pose.

Ascend and Descend preserve facing.

Advance captures facing at activation.

A/D input during an active Technique:

- does not steer it;
- does not reverse Advance;
- does not silently change facing.

---

# Combat Authority

Exactly one system owns gameplay movement:

1. ordinary movement;
2. Normal Technique;
3. Primed Technique;
4. authored hit or environmental reaction;
5. defeat.

Visual animation may catch up to authority changes.

Visual smoothing may not delay gameplay response, hit geometry, or control.

---

# Prime

Prime triggers only on a fresh press of the horizontal combat arrow opposite facing.

Prime may be armed:

- during ordinary movement;
- during an active Normal Technique.

Arming Prime during a Normal Technique does not interrupt that Technique.

The current Technique continues.

The next valid Advance, Ascend, or Descend becomes Primed.

Once Prime is armed:

- further Prime presses do nothing;
- the timer does not refresh;
- it does not stack;
- it does not extend;
- it cannot store multiple Primed Techniques.

Prime ends when:

- consumed;
- cleared by damage;
- cleared by an explicit transient-state reset;
- its original timer expires.

Prime input during:

- a Primed Technique;
- a hit reaction;
- another control-withholding state;

is ignored and not buffered.

Use an initial externally tunable duration around 1.5 seconds.

---

# Terrain

Use a custom kinematic controller.

Supported terrain:

- axis-aligned solid rectangles;
- one-way platforms;
- authored moving platforms;
- explicit hazards.

Do not implement:

- slopes;
- curved collision;
- polygonal rigid bodies;
- ragdolls;
- a general physics engine.

Art may imply angled machinery while gameplay collision stays axis-aligned.

Use swept or continuous collision checks for high-speed Techniques.

---

# Rooms and Stages

Build stages as linear sequences of authored rooms.

Each room may define:

- terrain;
- hazards;
- enemies;
- camera bounds;
- camera subregions;
- entry and exit points;
- music state;
- lighting;
- dialogue or transition behavior.

Boss arenas are dedicated rooms.

Safe transition rooms may contain dialogue.

Outward progression is effectively one-way.

Do not implement:

- general backtracking;
- farming;
- enemy respawn loops;
- persistent revisitation systems.

Combat rooms may lock forward progress until cleared.

The shutdown return reuses selected rooms with authored alternate:

- routes;
- lighting;
- music;
- enemy state;
- encounter composition.

---

# Camera

Ordinary rooms use smooth follow within authored subregion locks.

Large rooms may transition smoothly between authored camera regions.

Boss arenas must remain fully visible at all times.

Each boss uses one fixed authored zoom for the complete fight.

Do not dynamically zoom during combat.

Screen shake:

- is cosmetic;
- is brief;
- never changes gameplay coordinates;
- never obscures critical telegraphs;
- respects the settings value.

---

# Ordinary Body Collision

During ordinary movement:

- Irys and enemies use firm kinematic blocking;
- Irys cannot walk through enemies;
- enemies cannot walk through Irys;
- passive overlap does not push either participant;
- passive overlap causes no damage.

Authored attacks own displacement.

During Techniques:

- ordinary enemy-body blocking is ignored;
- Irys passes through enemies according to Player Feel;
- solid terrain remains authoritative;
- contact-adjusted clearance may carry Irys beyond the enemy.

---

# Passive Contact

Passive contact with an enemy body is harmless.

Damage comes only from:

- active enemy attacks;
- active boss attacks;
- explicit hazards;
- explicitly dangerous boss geometry.

Do not add generic touch damage.

---

# Player Attack Volumes

Use authored swept weapon volumes.

Use simple:

- capsules;
- rectangles;
- sequences of capsules or rectangles.

The active volume follows:

- Ion’s path;
- the active weapon hand;
- the authored pose;
- bounded generated reach.

Do not use pixel transparency for collision.

The visible sword trail must closely correspond to the damaging volume.

Developer tools must display:

- sweep volumes;
- hurtboxes;
- hit records;
- eligibility state.

---

# Player Hit Eligibility

One Technique activation may hit each eligible target once unless explicitly authored as multi-hit.

One activation may hit multiple different targets.

Continued overlap does not repeat damage.

A fresh activation while still overlapping the same target does not immediately damage it again.

Require:

1. full separation;
2. fresh re-entry;
3. new contact.

Maintain a per-activation target-ID set and overlap/separation state.

---

# Enemy and Boss Attack Hits

Each enemy or boss attack activation may damage Irys once unless explicitly authored as multi-hit.

Lingering overlap from one activation cannot repeatedly deal damage.

Explicit multi-hit attacks define:

- separate hit events;
- readable timing;
- separate active windows;
- appropriate telegraphs.

Different enemies or distinct attacks may hit during the same player reaction.

There are no universal post-hit invulnerability frames.

Fairness comes from attack design, spacing, displacement, readable windows, and encounter composition.

---

# Incoming Damage

Enemy and boss attacks deal fixed authored damage.

Every Irys frame has equal damage resistance.

Do not implement:

- armor;
- defense;
- mitigation percentages;
- elemental resistance;
- damage-type matrices;
- random incoming critical hits.

A 20-damage attack removes 20 structural integrity from every frame.

Frames may differ in maximum and current structural integrity.

---

# Knockback

Enemy and boss attacks define fixed authored displacement.

The same attack applies the same knockback to every Irys frame.

Do not generate displacement resistance.

Do not scale knockback by:

- mass;
- stature;
- anatomy;
- health;
- collision profile.

---

# Hit Recovery

Hit-reaction lockout may vary by frame.

The attack defines the reaction and displacement.

The frame’s surfaced hit-recovery property modifies only the duration before control returns.

It does not alter:

- damage;
- knockback;
- direction;
- terrain response.

Shorter, lighter, compact, or well-balanced frames generally recover faster.

Taller, heavier, elongated, or unstable frames generally recover more slowly.

Keep the range bounded so all frames remain responsive and viable.

---

# No I-Frames

Do not grant:

- post-hit invulnerability;
- dodge invulnerability;
- Technique invulnerability;
- Prime invulnerability.

A specifically authored action may avoid an attack only through its actual movement, collision, interruption behavior, or an explicit special rule.

---

# Player Damage and Crits

Each Technique defines base damage.

The frame’s strike-force property modifies damage.

Each eligible Technique hit may critically hit, including against bosses.

Use:

- one crit roll per target per Technique activation;
- surfaced critical-hit chance;
- surfaced critical-hit multiplier.

Prime does not grant a universal crit bonus.

Primed Techniques may crit normally unless explicitly authored otherwise.

Do not implement:

- critical failures;
- glancing blows;
- hidden crit resistance;
- boss crit immunity.

Initial tuning may begin near:

- 3–18% crit chance;
- 1.4×–1.8× crit multiplier.

These remain tunable rather than immutable.

Crit presentation may use:

- sharper Ion light;
- distinct impact sound;
- brighter internal fracture;
- brief diagnostic marker.

Do not use player-centered hit-stop.

---

# Boss Hurtboxes

Bosses may use multiple authored hurtboxes.

Possible regions:

- body;
- limbs;
- weapon;
- weak region;
- phase-specific region;
- dangerous collision geometry.

All damage feeds one persistent boss health pool.

Boss 3’s limbs may be separately targetable while sharing the same health bar.

Keep separate:

- hurtboxes;
- body collision;
- attack volumes;
- dangerous geometry.

Phase logic may enable, disable, move, or replace regions.

Do not implement pixel-perfect boss collision or generalized destructible bodies.

---

# Body Generation

Use deterministic constrained generation.

Do not roll every anatomy trait and statistic independently.

Generation sequence:

1. Select a curated structural archetype.
2. Select one authored anatomy configuration.
3. Select dominant side.
4. Generate bounded physical construction.
5. Apply named traits.
6. Generate continuous statistics within ranges constrained by the build.
7. Derive final gameplay values.
8. Validate viability, collision, rig compatibility, and complete Language support.

Possible internal archetypes include:

- balanced;
- lightweight;
- dense;
- compact;
- elongated;
- reinforced;
- unstable;
- precision-oriented.

Archetypes are generation machinery and do not need to be presented as player classes.

Examples of coherent dependency:

- compact bodies tend toward quicker hit recovery;
- elongated limbs tend toward greater reach;
- dense bodies may support greater structural integrity;
- precision construction may support higher crit chance;
- lightweight construction may support acceleration and air control;
- unstable construction may worsen recovery.

Do not normalize every frame to one hidden power budget.

Frames may be weak, strong, specialized, or exceptional.

Every frame remains viable.

---

# Generated Properties

Generated frame properties are:

1. Maximum structural integrity.
2. Ground speed.
3. Ground acceleration.
4. Jump power.
5. Air control.
6. Hit-reaction recovery.
7. Physical reach.
8. Strike force.
9. Critical-hit chance.
10. Critical-hit multiplier.
11. Advance distance.
12. Ascend height.
13. Descend drive.
14. Prime hold duration.

Universal across all frames:

- damage resistance;
- enemy-authored knockback;
- gravity;
- base falling behavior;
- Normal Technique duration;
- Language vocabulary;
- interruption laws;
- terrain collision laws;
- hit eligibility rules.

Do not include displacement resistance as a generated property.

---

# Anatomy Configurations

The first implementation supports exactly four authored body configurations:

1. Standard.
2. SingleArm.
3. SingleLeg.
4. AdditionalArm.

Do not build an arbitrary-limb rig.

Each configuration requires:

- complete authored rig support;
- compatible modular parts;
- intentional sealed anatomy;
- adapted Ark suit construction;
- coherent generation constraints;
- gameplay collision compatibility;
- locomotion animation;
- jump and landing animation;
- damage poses;
- Normal Technique poses;
- Primed Technique poses;
- inspection-screen rendering;
- left- and right-facing support.

These are intentional Ark specializations.

They must not look like failed assembly or forgotten limbs.

---

# SingleArm

SingleArm is purpose-built.

Use:

- sealed shoulder architecture;
- purpose-built asymmetrical torso and suit panels;
- rerouted luminous channels;
- adjusted balance;
- authored one-arm locomotion;
- authored left- and right-arm variants.

Either arm may remain.

Ion must be wieldable in either hand.

---

# SingleLeg

SingleLeg uses a purpose-built wheel-foot monopod lower body.

Use:

- integrated rolling wheel-foot;
- stable standing and bracing structure;
- small rear thruster;
- authored rolling locomotion;
- authored braking;
- thruster-assisted jump and balance correction;
- adapted landing and Technique poses.

The thruster supports:

- acceleration;
- braking;
- jumping;
- stabilization;
- Technique presentation.

It does not provide continuous hovering.

Do not depict a human pelvis with one ordinary leg simply absent.

---

# AdditionalArm

AdditionalArm uses one authored third arm attached at a shoulder.

Do not use arbitrary limb placement.

Any of the three arms may wield Ion.

The body may automatically pass Ion between arms during:

- Techniques;
- transitions;
- recovery;
- balance corrections;
- visual flourishes.

The player does not control hand selection.

Hand changes do not create:

- new inputs;
- a new Technique;
- multiple simultaneous Ion attacks;
- dual wielding;
- additional hit eligibility.

Ion remains one weapon with one authored sweep.

---

# Handedness

Every generated body has a dominant side:

- left;
- right.

Either side may wield Ion.

A SingleArm body may retain either the dominant or non-dominant arm.

Dominant and non-dominant use may create small surfaced differences in handling, strike expression, recovery, or control.

Every configuration remains viable.

Support authored mirrored or paired weapon poses where needed.

AdditionalArm may gain modest benefits from its ability to assign or transfer Ion between multiple arms, without creating a new combat vocabulary.

---

# Collision Profiles

Use three bounded player hurtbox profiles:

- small;
- standard;
- large.

Derive the profile from stature, mass, and build.

Exact visual anatomy may vary more widely.

Do not generate an arbitrary collision mesh for each body.

Visible limb length may still affect:

- attack reach;
- Technique clearance;
- animation;
- inspection presentation.

Every collision profile must function through every required route and boss arena.

---

# Modular Rendering and Animation

Use a fixed core authored skeleton with optional authored branches for the four configurations.

Use:

- modular pixel-art parts;
- authored key poses;
- controlled bone-length variation;
- sprite substitution;
- smear frames or smear shapes;
- trails;
- alternate poses;
- limited controlled interpolation.

Do not implement:

- runtime inverse kinematics as a dependency;
- ragdolls;
- generalized procedural animation;
- arbitrary limb generation.

Each anatomy configuration requires its own ordinary locomotion family.

Generated speed, acceleration, mass, and balance may adjust playback and bounded pose parameters.

Do not create a separate animation set for every statistical roll.

Animation must not resemble loose rotating paper-doll parts.

---

# Enemy and Boss Decision Making

Use authored state machines with contextual weighted attack selection.

Attack choice is randomized, especially for bosses.

Validity and weighting may depend on:

- phase;
- distance;
- elevation;
- cooldowns;
- arena state;
- current positioning;
- recent attack history;
- whether the attack is usable from the current state.

Use anti-repeat weighting.

Once selected, an attack’s:

- telegraph;
- timing;
- movement;
- hitboxes;
- damage;
- recovery;

remain fixed.

Enemies and bosses may not:

- secretly replace an attack after telegraphing;
- read the player’s current input to choose an unavoidable counter;
- mutate attack timing randomly;
- create impossible combinations.

Use the deterministic PRNG so behavior can be reproduced in developer mode.

---

# Difficulty

Ship one canonical difficulty only.

Target:

- hard to very hard.

Do not implement:

- Easy/Normal/Hard selection;
- dynamic difficulty;
- adaptive damage scaling;
- hidden assistance;
- public boss practice.

Developer tools may jump directly to bosses.

Difficulty must come from:

- readable authored attacks;
- demanding timing;
- positioning;
- attrition;
- boss knowledge;
- mastery of The Language.

Accessibility settings may reduce sensory intensity but not secretly alter challenge values.

---

# Settings and Accessibility

Initial persistent settings include:

- master volume;
- music volume;
- effects volume;
- dialogue-tone volume;
- fullscreen/windowed mode;
- resolution handling;
- integer scaling;
- screen-shake intensity;
- flash/effect intensity;
- camera smoothing;
- dialogue text speed.

Pause must be supported.

Deferred:

- key rebinding;
- controller configuration;
- colorblind presets;
- broad accessibility framework.

Settings must not alter core difficulty.

---

# Developer Mode

Developer mode is required.

It must be hidden or disabled in normal play.

Include practical support for:

- JSON hot reload;
- hitbox display;
- hurtbox display;
- attack-volume display;
- collision display;
- active Technique;
- combat authority;
- armed Prime and remaining time;
- player velocity;
- enemy state;
- boss state and phase;
- current room;
- campaign state;
- generated frame properties;
- traits;
- anatomy configuration;
- dominant side;
- current weapon hand;
- collision profile;
- run seed;
- frame time;
- fixed-step diagnostics;
- invulnerability cheat for testing;
- room restart;
- boss restart;
- direct room selection;
- direct boss selection;
- single-step simulation while paused;
- profile/stat reset options.

Developer invulnerability is a testing cheat and does not alter the canonical no-i-frame gameplay rule.

---

# Testing

Do not mandate a large automated test framework.

Explicitly authorize and encourage Sol to write and run targeted tests when they materially reduce risk.

Likely candidates include:

- deterministic body generation;
- configuration validation;
- Prime lifecycle;
- combat-authority transitions;
- per-activation hit eligibility;
- frame swapping;
- campaign-state transitions;
- profile serialization;
- save recovery.

Sol decides what tests are useful.

Testing must not displace completion of the full game.

Sol must report:

- tests written;
- commands run;
- results;
- unresolved failures.

Visual quality, combat feel, boss readability, animation, and camera behavior require manual playtesting.

---

# Art and Asset Pipeline

Runtime loads normal assets such as:

- PNG sprite sheets;
- modular body-part atlases;
- fonts;
- music;
- sound effects;
- UI assets;
- data files.

Sol may use deterministic asset-generation scripts where useful.

Generated assets must be stored and loaded normally at runtime.

The pipeline must allow later replacement by professionally authored assets without rewriting gameplay systems.

The first implementation must include a coherent asset set for the complete campaign.

Do not stop after Boss 1.

---

# Audio

Implement the approved Audio Direction.

Use a small authored reactive layering system.

Ordinary stages retain one continuous musical identity with states such as:

- exploration;
- tension;
- combat;
- post-combat release.

Bosses use dedicated authored tracks.

Do not create granular music reaction to every input.

Audio mix and layer values belong in tuning data.

---

# HUD

The standard HUD remains visible during gameplay.

The boss health bar is the only ordinary HUD element whose visibility changes according to encounter state.

HUD must show the essential current state clearly, including:

- structural integrity;
- Prime state;
- other required surfaced body or combat information.

Do not consume excessive combat view area.

---

# Scope Boundaries

Do not implement unless required by an approved owning specification:

- ECS;
- general physics;
- slopes;
- arbitrary limbs;
- procedural stages;
- active-run saves;
- controller support;
- key rebinding;
- multiple profiles;
- multiple difficulties;
- public boss practice;
- permanent upgrades;
- loot;
- card systems;
- equipment;
- dynamic boss scaling;
- armor or mitigation;
- displacement resistance;
- post-hit i-frames;
- touch damage;
- random attack timing;
- complex test infrastructure;
- an editor;
- scripting;
- online services;
- cloud saves;
- achievements as gameplay progression;
- post-clear secret content.

---

# Definition of Technical Success

The implementation is technically successful when:

- it builds and runs on macOS;
- the full fixed campaign is playable;
- all four body configurations function;
- generated frames are coherent and viable;
- the complete Language works;
- all five bosses are implemented;
- post-boss frame replacement works;
- run failure returns to the Ark;
- the final ending can be reached;
- settings and statistics persist;
- no active run persists after exit;
- developer tools support tuning;
- the game uses coherent first-pass presentation;
- the architecture remains portable to Windows and Linux;
- no major creative or architectural decision is left to the implementation session.
