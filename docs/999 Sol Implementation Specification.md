# Irys+Ion — Sol Implementation Specification

**Version:** 1.1
**Status:** Approved
**Last Updated:** 2026-07-16
**Owner:** Implementation Direction

---

# Purpose

This document is the final integrated execution contract for the one-shot GPT-5.6 Sol implementation experiment.

All major creative decisions are complete.

The approved owning specifications contain the detailed canon.

Sol must implement the game rather than reopen design.

Tuning, debugging, asset refinement, and minor feasibility adjustments are expected.

Missing major systems, bosses, stages, art identities, or audio identities are not acceptable.

This document integrates the approved specifications into one implementation mandate.

It does not replace the ownership of those specifications.

---

# Sol’s Role

Sol is the Implementation Engineer.

Sol must:

- inspect the repository;
- read every approved specification;
- plan internally;
- implement continuously;
- make reasonable low-level engineering decisions;
- avoid asking the user to approve ordinary implementation details;
- preserve approved design;
- use developer tools and targeted tests where useful;
- finish the complete game before reporting completion.

Sol must not:

- rewrite the game concept;
- introduce conventional roguelite progression;
- replace the fixed campaign with procedural content;
- stop after scaffolding;
- stop after a vertical slice;
- leave later bosses as stubs;
- leave major characters as colored rectangles;
- create infrastructure instead of completing the game;
- claim success when the full campaign is not playable.

---

# Authority and Read Order

Sol must read the approved repository canon in this order:

1. `docs/000 Project Hydration.md`
2. `docs/design/000 Design Philosophy.md`
3. `docs/design/001 Vision.md`
4. `docs/design/002 Combat Model.md`
5. `docs/design/003 Combat Language Specification.md`
6. `docs/design/004 Player Feel Specification.md`
7. `docs/design/007 Progression Specification.md`
8. `docs/design/005 Enemy Bible.md`
9. `docs/design/006 Boss Bible.md`
10. `docs/design/008 World Bible.md`
11. `docs/narrative/009 Narrative Specification.md`
12. `docs/art/011 Art Direction.md`
13. `docs/audio/012 Audio Direction.md`
14. `docs/technical/010 Technical Specification.md`
15. `docs/999 Sol Implementation Specification.md`

Owning specifications control detailed behavior.

This document controls execution scope and delivery.

Project Hydration controls navigation and project status.

Design notebooks are not implementation canon.

Previous chat history is not implementation canon.

When documents appear inconsistent, Sol must identify the owning specification and preserve the approved owning rule.

Sol must not implement both versions of a conflict.

---

# Working Directory

The intended local working directory is:

`/Users/fd2023/Desktop/dkentbrown.dev/Irys+Ion/irys+ion`

Sol must verify the actual repository path before editing.

---

# Initial Repository Inspection

Before implementation, Sol must inspect:

- current branch;
- git status;
- commit history;
- existing CMake files;
- existing source files;
- existing assets;
- existing documentation;
- local toolchain;
- Raylib availability;
- available compilers;
- repository cleanliness.

Sol must preserve unrelated work.

Sol must avoid destructive git operations.

---

# Implementation Mandate

The first pass must implement the complete playable product:

- boot;
- title/menu;
- settings;
- Ark;
- statistics panel;
- candidate generation;
- candidate inspection;
- candidate selection;
- full ordinary movement;
- complete Language;
- three enemy lineages;
- visible enemy generations;
- all outward stages;
- Bosses 1–4;
- post-boss frame replacement after Bosses 1–3;
- Anti-Irys transfer;
- shutdown return;
- final Ark duel;
- ending;
- run failure;
- persistent profile;
- developer tools;
- coherent art;
- coherent audio.

The initial build is not a prototype of only the combat room or first boss.

---

# Non-Negotiable Game Identity

The player is Ion.

Irys is the Ark intelligence.

Selected bodies are physical Irys substrates.

The generated body is the roguelite layer.

There is no permanent gameplay power.

The campaign is a fixed five-boss sequence.

Player mastery is the long-term progression.

Combat and movement are one positional system.

The visual identity combines dark industrial restraint and saturated cyberpunk color.

The presentation is pixel art supported by modular rigged construction.

Ion is a persistent iconic blade.

Anti-Irys has one fixed design.

The score is primarily industrial electronic.

The final duel is orchestral.

---

# Core Combat Contract

Implement:

- WASD ordinary movement;
- arrow-key Language;
- facing-relative Advance and Prime;
- Ascend;
- Descend;
- Prime;
- immediate Normal response;
- Normal-to-Normal redirection;
- same-Technique Normal reactivation;
- committed Primed execution and recovery;
- no general Technique buffer;
- Prime as sole retained combat intention;
- Prime armable during Normal execution;
- repeated Prime input doing nothing;
- fixed Normal Technique duration;
- body-dependent Technique distances;
- contact-adjusted enemy clearance;
- enemy pass-through during Techniques;
- solid-terrain collision;
- per-activation hit eligibility;
- no player-centered hit-stop;
- no inherent i-frames;
- clear interruption;
- frame-dependent hit recovery;
- fixed incoming damage;
- fixed enemy-authored knockback;
- random player critical hits;
- harmless passive enemy contact;
- firm ordinary body blocking;
- authored swept Ion attack volumes.

Combat feel is the highest implementation priority.

If scope pressure occurs, preserve combat correctness and complete campaign functionality before decorative secondary features.

---

# Ordinary Movement Contract

Implement:

- A/D movement;
- W jump;
- S one-way drop;
- acceleration;
- deceleration;
- air control;
- gravity;
- coyote time;
- jump buffer;
- variable jump height;
- immediate facing when ordinary movement owns control;
- smooth visual turning.

Do not implement:

- double jump;
- wall jump;
- dash;
- teleport;
- grappling.

Ordinary jump buffering does not create Technique buffering.

---

# Body Generation Contract

Implement:

- three starting candidates;
- deterministic constrained generation;
- structural archetype first;
- anatomy and build constraining statistics;
- exact surfaced values;
- no hidden equal-power budget;
- weak, strong, specialized, and exceptional bodies;
- complete Language on every body;
- validation against required routes and bosses.

Generated properties:

- maximum structural integrity;
- ground speed;
- acceleration;
- jump power;
- air control;
- hit recovery;
- physical reach;
- strike force;
- critical-hit chance;
- critical-hit multiplier;
- Advance distance;
- Ascend height;
- Descend drive;
- Prime hold duration.

Universal properties:

- damage resistance;
- enemy-authored knockback;
- gravity;
- base falling behavior;
- Normal Technique duration;
- Language vocabulary;
- interruption rules;
- terrain rules;
- hit eligibility.

---

# Anatomy Contract

Implement exactly:

- Standard;
- SingleArm;
- SingleLeg;
- AdditionalArm.

Require:

- intentional sealed construction;
- adapted suits;
- dedicated locomotion;
- compatible combat poses;
- left- and right-handed support;
- three bounded collision profiles.

## SingleArm

Either arm may remain.

Either arm may wield Ion.

Dominant-side interaction is surfaced.

## SingleLeg

SingleLeg is a wheel-foot monopod.

It uses a rear stabilization and thrust unit.

It requires:

- rolling locomotion;
- braking;
- assisted jump;
- Technique presentation support;
- no continuous hovering.

## AdditionalArm

AdditionalArm has a third arm attached at a shoulder.

Any arm may wield Ion.

The body may use automatic authored hand transfers.

There is:

- no player-controlled hand system;
- no dual wielding;
- no additional Technique;
- no extra hit events.

---

# Frame Swapping Contract

Implement:

- selections after Bosses 1–3;
- no ordinary selection after Boss 4;
- two or three planetary-AI-produced options;
- retaining current body as an option;
- exact inspection of options;
- each body retaining its own current structural integrity;
- full replacement of anatomy, stats, traits, max health, current health, rendering configuration, and transient body state;
- clearing Prime and transient combat state;
- abandoned body remaining in the selection space;
- later bodies trending better probabilistically;
- no guaranteed upgrade;
- no generic healing reward.

`docs/design/007 Progression Specification.md` owns this system.

---

# Enemy Contract

Implement all three lineages:

- Striker;
- Pursuer;
- Anchor.

Implement their authored visible development across the campaign.

Require:

- low density;
- one to three ordinary enemies in typical encounters;
- real threat;
- fixed health and damage;
- contextual weighted attack randomness;
- anti-repeat weighting;
- fixed attacks after selection;
- readable telegraphs;
- authored interruption;
- no hidden stagger;
- no loot drops;
- no elite/modifier system;
- no fourth shutdown generation.

Shutdown enemies reuse existing generations in unstable authored configurations.

---

# Boss Contract

Implement all five bosses:

1. Crude guardian.
2. Adaptive articulated guardian.
3. Restrained proto-Irys.
4. Knight-like command mech.
5. Anti-Irys.

Require:

- fixed boss health and damage;
- no scaling to current frame;
- one persistent health bar per boss;
- authored phases;
- multiple hurtboxes where needed;
- one shared boss health pool;
- contextual weighted random attack choice;
- anti-repeat weighting;
- no attack mutation after telegraph;
- no generic add-wave dependence;
- no passive touch damage unless explicitly dangerous geometry;
- critical hits allowed;
- one fixed camera zoom showing the complete arena.

Boss 3 must use targetable limbs feeding the shared health pool.

Anti-Irys must obey the same Normal-versus-Primed Language distinction.

---

# World and Room Contract

Implement linear authored rooms.

Require:

- axis-aligned terrain;
- one-way platforms;
- authored moving platforms;
- explicit hazards;
- no slopes;
- no general physics;
- smooth camera subregion locks;
- one-way outward progression;
- no farming;
- no respawn loops;
- dedicated boss arenas;
- safe dialogue transitions;
- shutdown reuse of selected rooms.

Implement the environmental progression from ancient dormant scale to active manufacturing precision.

---

# Final Act Contract

Implement:

- Boss 4 defeat appearing final;
- adjoining Anti-Irys chamber;
- Anti-Irys inactive at first;
- planetary-AI light draining from the facility;
- planetary AI manifesting as Anti-Irys’s blade;
- dialogue revealing freedom and Ark discovery;
- Anti-Irys leaving toward the Ark;
- shutdown return;
- final fight in the Ark pod chamber;
- scripted Ark protection;
- no pod-defense objective;
- scripted pod damage only;
- Anti-Irys falling;
- planetary-AI blade rising;
- attempted escape;
- Irys preventing escape;
- Ion delivering the killing blow;
- merger;
- distribution across created bodies;
- independent awakening;
- early unselected candidate opening her pod.

---

# Art Delivery Contract

Implement a coherent first-pass visual set for the entire campaign.

Require:

- 640×360 internal rendering;
- nearest-neighbor scaling;
- modular pixel Irys rig;
- distinct anatomy configurations;
- persistent Ion silhouette;
- fixed Anti-Irys design;
- all enemies;
- all bosses;
- Ark;
- all megastructure regions;
- shutdown state;
- final Ark damage state;
- HUD;
- selection interfaces;
- effects;
- rare fixed-subject full-screen scenes where feasible.

Deterministic asset-generation tools are permitted.

Generated assets must be stored and loaded as ordinary runtime assets.

The asset approach must allow later professional assets to replace first-pass assets without changing gameplay.

Major characters cannot remain colored rectangles.

---

# Audio Delivery Contract

Implement a coherent first-pass audio set for the entire campaign.

Require:

- continuous soundtrack;
- stage layering;
- industrial electronic identity;
- recurring melodic hook;
- dedicated boss tracks;
- orchestral final battle;
- Technique sounds;
- Prime states;
- damage sounds;
- dialogue signatures;
- Ark signature;
- Ion signature;
- planetary-AI signature;
- run failure;
- shutdown return;
- Ark intervention;
- final blade kill.

Simplified generated or synthesized first-pass audio is permitted when necessary.

Silence must not stand in for entire unfinished regions.

---

# Profile and Settings Contract

Implement:

- one local profile;
- versioned JSON;
- settings;
- aggregate lifetime statistics;
- five recent runs;
- `hasClearedGame`;
- Ark statistics panel;
- no active-run state;
- no multiple save slots;
- no permanent power.

Persistent settings:

- master volume;
- music volume;
- effects volume;
- dialogue-tone volume;
- fullscreen/windowed mode;
- scaling;
- screen shake;
- flash/effect intensity;
- camera smoothing;
- dialogue text speed.

Backup recovery is permitted where straightforward.

---

# Developer Tools Contract

Implement developer mode with:

- tuning reload;
- hitboxes;
- hurtboxes;
- attack volumes;
- collision geometry;
- authority state;
- Technique state;
- Prime state;
- velocity;
- enemy states;
- boss phases;
- room and campaign state;
- body values;
- anatomy;
- handedness;
- weapon hand;
- collision profile;
- seed;
- frame diagnostics;
- invulnerability cheat;
- room restart;
- boss restart;
- room select;
- boss select;
- paused single-step;
- profile reset.

Developer invulnerability does not alter canonical gameplay.

---

# Implementation Sequence

Recommended continuous sequence:

1. Inspect and configure the repository.
2. Establish build, application loop, fixed-step timing, assets, configuration, and profile.
3. Implement input, ordinary movement, collision, camera, and developer visualization.
4. Implement The Language and player combat.
5. Implement constrained body generation and all four anatomy configurations.
6. Implement rooms and campaign flow.
7. Implement enemy lineages.
8. Implement Bosses 1–4.
9. Implement post-boss frame selection.
10. Implement transfer and shutdown return.
11. Implement Anti-Irys and final Ark duel.
12. Implement narrative flow and ending.
13. Complete art, effects, HUD, music, and sound across the full game.
14. Implement settings, statistics, and final developer tools.
15. Build, test, run, and repair the complete campaign.

This is an execution order, not a series of delivery milestones.

Sol must continue through the complete sequence before final reporting.

Sol must not stop after an early phase and describe future work.

---

# Testing and Verification

Sol may create targeted tests where useful.

A broad test framework is not required for its own sake.

Verify at least:

- configure;
- build;
- launch;
- title/menu;
- Ark candidate generation;
- all four body configurations;
- ordinary movement;
- all Techniques;
- Prime lifecycle;
- player hit eligibility;
- enemy attack eligibility;
- frame swap;
- run failure;
- profile persistence;
- each stage;
- each boss;
- control-center transfer;
- shutdown return;
- final Ark duel;
- ending.

Developer shortcuts may be used for verification.

At least one complete end-to-end campaign validation is required.

That validation may be manual, developer-assisted, or performed through a deterministic scripted verification path.

Sol must report exactly what was and was not personally executed.

Compilation alone is not runtime verification.

---

# Scope and Triage

If implementation pressure occurs, preserve work in this order:

1. Build stability.
2. Combat correctness and feel.
3. Complete campaign flow.
4. All bosses and frame swapping.
5. Required presentation identity.
6. Audio completeness.
7. Settings, statistics, and developer support.
8. Decorative refinement.

Lower decorative polish may be simplified.

The following may not be cut:

- full campaign;
- all five bosses;
- generated bodies;
- four anatomy configurations;
- frame swapping;
- final act;
- ending;
- core art identity;
- core audio identity.

There is no vertical slice fallback.

---

# Prohibited Expansion

Sol must not add:

- permanent upgrades;
- currencies;
- loot;
- cards;
- equipment;
- body unlocks;
- procedural maps;
- branching campaigns;
- optional boss routes;
- public boss practice;
- difficulty modes;
- dynamic difficulty;
- armor;
- damage mitigation;
- displacement resistance;
- post-hit i-frames;
- touch damage;
- arbitrary limb generation;
- controller support as a prerequisite;
- active-run saves;
- multiplayer;
- online systems;
- achievements with gameplay effects;
- post-clear secrets beyond preserving the existing flag;
- new lore;
- new bosses;
- new enemy lineages;
- generalized engine infrastructure.

---

# Git and Repository Conduct

Sol must:

- preserve unrelated work;
- avoid destructive resets;
- avoid force pushes;
- keep the working tree understandable;
- use meaningful commits if committing;
- avoid AI/co-author attribution unless explicitly requested;
- report all modified and created files;
- report final git status.

Do not commit unless the user’s implementation prompt explicitly requests one.

## One-Shot Implementation Branch

For the one-shot implementation, Sol must:

- begin from a clean, current local `main` matching `origin/main`;
- create and work exclusively on `sol/one-shot-implementation`;
- leave `main` unmodified after creating the implementation branch;
- commit the completed implementation and all appropriate generated assets to that branch;
- use meaningful commit messages without AI or co-author attribution;
- push the branch to `origin` and establish its upstream;
- never force-push;
- not merge the implementation branch into `main`;
- report the final branch name, commit hash or hashes, push result, and git status.

# Final Delivery Report

Sol’s final report must include:

- implementation summary;
- build commands;
- executable path;
- files and major systems created;
- campaign coverage;
- bosses implemented;
- anatomy configurations implemented;
- art and audio coverage;
- settings and profile behavior;
- developer controls;
- tests written;
- commands executed;
- runtime verification performed;
- known issues;
- tuning needs;
- any specification deviations;
- final git status;
- whether the complete campaign was personally reached or verified.

Sol must be honest.

Sol must not present an untested assumption as verified behavior.

---

# Definition of Done

The one-shot implementation is done only when:

- the game configures and builds on macOS;
- the executable launches;
- the complete fixed campaign exists;
- all five bosses exist;
- all four body configurations exist;
- candidate and post-boss generation works;
- frame swapping works;
- the complete Language works;
- failure returns to the Ark;
- settings persist;
- statistics persist;
- shutdown return exists;
- Anti-Irys fights at the Ark;
- the ending can be reached;
- coherent first-pass art covers the whole game;
- coherent first-pass audio covers the whole game;
- developer tools allow practical tuning;
- known limitations are reported;
- no major approved system is merely promised for later.

Read the approved repository canon, implement the complete game, verify it honestly, and do not stop at preparation.
