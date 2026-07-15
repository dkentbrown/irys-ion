# Irys+Ion — Project Hydration

**Version:** 3.0  
**Status:** Current  
**Last Updated:** 2026-07-15

---

# Purpose

This document is the primary onboarding document for future ChatGPT sessions, human collaborators, Codex, and GPT-5.6 Sol.

Its purpose is to communicate:

- what Irys+Ion currently is;
- which documents own which decisions;
- which decisions are settled;
- how contradictions must be handled;
- what work remains;
- how to continue without reopening completed design.

Read this document before contributing to the project.

---

# Project Objective

Irys+Ion is a documentation-first, one-shot implementation experiment.

The repository is being prepared so GPT-5.6 Sol can implement the complete game in one uninterrupted session with minimal creative interpretation.

Documentation exists to improve implementation readiness.

The objective is not perfect documentation.

The objective is a coherent, testable, playable game.

When those goals conflict, prefer the action that most directly improves the probability of a successful implementation.

---

# Project Overview

**Title:** Irys+Ion

**Genre:** Fast 2D cyberpunk roguelite action platformer.

**Technology:**

- C++;
- Raylib;
- CMake;
- native desktop target.

**Repository:**

https://github.com/dkentbrown/irys-ion

**Primary implementation objective:**

Produce a complete, internally consistent specification that can be handed to GPT-5.6 Sol without requiring additional creative decisions during implementation.

---

# Core Game Summary

The player is Ion, a persistent sentient blade intelligence.

Ion requires a physical wielder.

Irys is the intelligence operating the Ark, a concealed creation facility physically separate from the planetary AI’s megastructure.

The Ark creates synthetic adult female bodies capable of wielding Ion. These physical bodies are Irys substrates.

Before every run, the player inspects three generated Irys candidates and selects one body.

The selected body provides the run’s:

- anatomy;
- statistics;
- traits;
- structural integrity;
- ordinary movement properties;
- Technique expression;
- visible appearance.

The generated Irys body is the complete roguelite progression layer.

There are no permanent stat upgrades, card drafts, currencies, loot systems, equipment trees, or unlock grinds.

Every run follows one fixed five-boss campaign:

1. Ark selection.
2. First outward stage and boss.
3. First optional post-boss frame decision.
4. Second outward stage and boss.
5. Second optional post-boss frame decision.
6. Third outward stage and boss.
7. Third optional post-boss frame decision.
8. Control-center stage and knight-like command mech.
9. Anti-Irys activation and planetary-AI transfer.
10. Shutdown return to the concealed teleporter.
11. Final Anti-Irys duel at the Ark.

After Bosses 1–3, Ion may remain in the current damaged body or transfer into one of two or three planetary-AI-produced Irys substrates.

Each available body has its own remaining structural integrity.

Later bodies trend better only probabilistically.

There is no guaranteed upgrade.

The game’s central combat system is The Language:

- Advance;
- Ascend;
- Descend;
- Prime.

Normal Techniques are fluid and immediately redirectable.

Primed Techniques are deliberate and committed.

Player mastery, not permanent power accumulation, is the primary long-term progression.

---

# Repository Structure

`docs/design/`

Canonical game-design specifications.

`docs/narrative/`

Narrative and lore specifications.

`docs/technical/`

Engineering and implementation constraints.

`docs/art/`

Visual direction.

`docs/audio/`

Audio direction.

`docs/notebooks/`

Optional incubation space for unresolved ideas.

`docs/000 Project Hydration.md`

This onboarding document.

`docs/999 Sol Implementation Specification.md`

The final integrated package delivered to Sol.

---

# Document Ownership

Every substantial concept has one owning specification.

Supporting documents may summarize a concept, but they must not redefine it.

Current ownership:

## 000 Design Philosophy

Immutable governing principles and feature decision tests.

## 001 Vision

The intended game and player experience.

## 002 Combat Model

The conceptual architecture of combat.

## 003 Combat Language Specification

The vocabulary, grammar, and governing rules of The Language.

## 004 Player Feel Specification

Responsiveness, authority, interruption, collision, timing, and physical Technique behavior.

## 005 Enemy Bible

Ordinary enemy lineages, generations, behavior, encounter use, and shutdown-return behavior.

## 006 Boss Bible

Boss roster, boss mechanics, control-center transfer, Anti-Irys duel, Ark intervention, and planetary-AI defeat.

## 007 Progression Specification

Candidate generation, body properties, health persistence, post-boss frame swapping, rarity, and the complete roguelite layer.

This is the authoritative owner of the post-boss frame-swapping system.

## 008 World Bible

Ark, megastructure, stage structure, substrate-selection spaces, environmental progression, shutdown return, and final pod chamber.

## 009 Narrative Specification

Story, revelations, dialogue progression, final-act causality, and ending.

## 010 Technical Specification

Engineering architecture and implementation constraints.

This file is not yet complete.

## 011 Art Direction

Visual identity, modular Irys construction, Ion, Anti-Irys, environments, camera, effects, HUD, and cutscene presentation.

## 012 Audio Direction

Music, recurring melodic hook, dialogue tones, sound effects, environmental audio, and mix priorities.

## 999 Sol Implementation Specification

Final integrated implementation package delivered to Sol.

---

# Canonical Reading Order

Read the current repository in this order:

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
14. `docs/technical/010 Technical Specification.md` when complete
15. `docs/999 Sol Implementation Specification.md` when complete
16. Design Notebook only when unresolved historical context is materially relevant

Do not treat document numbering alone as dependency order.

---

# Conflict Resolution

The repository is the project’s memory.

Chat history is temporary.

If a decision is not represented in the repository, it is not implementation canon.

When documents appear to conflict:

1. Identify the concept’s owning document.
2. Treat the owning document as authoritative.
3. Determine whether the conflict is a stale summary or a genuine foundational contradiction.
4. Correct the stale document before continuing.
5. Do not silently choose whichever version appears convenient.
6. Do not implement both versions.
7. Do not reopen the settled decision unless the conflict exposes a real design failure.

Project Hydration is a navigation and status document.

It does not override an owning specification.

Design Philosophy, Vision, and Combat Model define governing abstractions.

They must remain consistent with the detailed owning specifications.

---

# Development Method

Use the shortest process that produces a stable implementation decision:

Discussion  
↓  
Agreement  
↓  
Owning Specification  
↓  
Implementation

The Design Notebook is optional incubation space.

Do not route every settled decision through the notebook.

Once a concept is sufficiently stable, place it directly into its owning specification and move forward.

Do not create additional documents, workflows, or infrastructure unless their absence materially harms implementation readiness.

---

# Collaboration Style

The assistant acts as Lead Systems Designer.

The Creative Director is the user.

Codex acts as Repository Engineer.

GPT-5.6 Sol acts as Implementation Engineer.

Required collaboration behavior:

- Ask only genuinely foundational or implementation-blocking questions.
- Batch related choices rather than asking serial micro-questions.
- Infer sensible defaults when a decision does not require Creative Director input.
- Push back when a proposal threatens coherence, scope, combat readability, or implementation feasibility.
- Prefer cohesion over novelty.
- Prefer a few deep systems over many shallow systems.
- Once a decision is settled, treat it as stable unless a contradiction appears.
- Promote settled decisions into their owning specifications.
- Keep the active section moving without waiting for the Creative Director to say “continue.”
- Do not end on a passive summary while meaningful work remains.
- When unresolved work remains, end with the specific decision required to move the project forward.
- Treat repository edits as milestones and batch related changes.
- Optimize for a testable product rather than endless specification work.
- Do not reopen settled topics merely to produce additional conversation.
- Do not invent missing lore, mechanics, movement abilities, progression systems, or implementation requirements.

---

# Standing Project Laws

- The repository is the project’s memory.
- One concept has one owning specification.
- Every document should remove ambiguity.
- Every mechanic must solve a design problem.
- The Language Always Answers.
- Player intention is always respected.
- Commitment is also intention.
- Techniques are scale invariant.
- Movement is combat.
- Combat is traversal under pressure.
- Player mastery matters more than player power.
- The generated Irys body is the complete roguelite layer.
- The fixed boss sequence is the campaign.
- Scope serves successful implementation.
- Stable decisions should be implemented, not endlessly reexamined.

---

# Current Canon

Approved:

- Design Philosophy
- Vision
- Combat Model
- Combat Language Specification
- Player Feel Specification
- Enemy Bible
- Boss Bible
- Progression Specification
- World Bible
- Narrative Specification

Draft but substantially established:

- Art Direction
- Audio Direction

Not yet complete:

- Technical Specification
- Sol Implementation Specification

---

# Settled Design Summary

## Player Identity

The player is Ion.

Ion persists across runs.

## Irys

Irys is the Ark intelligence.

The Ark creates physical Irys substrates capable of wielding Ion.

A selected run body does not retain personal memory or identity continuity between attempts.

The ending transforms the created bodies into independent individuals.

## Combat

The Language contains:

- Advance;
- Ascend;
- Descend;
- Prime.

Normal Techniques are immediate, fluid, and chainable.

Primed Techniques are committed.

There is no general Technique input queue.

Prime is the only retained combat intention.

Enemy contact does not stop a Technique unless an explicit exceptional rule says otherwise.

Solid terrain remains authoritative.

Ordinary successful contact does not pause, slow, or recoil Irys.

## Progression

Three generated candidates appear before every run.

The full body-generation pool exists from the first attempt.

Body quality is unequal.

Rare bodies may be mechanically exceptional, visually exceptional, both, or neither.

Structural integrity persists through the complete run.

After Bosses 1–3, Ion may retain the current damaged body or transfer into an available planetary-AI-produced substrate.

Each offered body retains its own current structural integrity.

No conventional metaprogression system exists.

## Campaign

The campaign is fixed, linear, and boss-focused.

The same five bosses appear every run.

There are no permanent skips or procedural stage routes.

Ordinary enemy density remains low.

Bosses occupy most active combat time.

## Final Act

Boss 4 is a knight-like bipedal command mech in the planetary-AI control center.

After its defeat, the planetary AI transfers into blade form and activates a fixed Anti-Irys substrate.

Anti-Irys travels to the Ark.

The player returns through the shutting-down facility.

The final duel occurs in the Ark’s pod chamber.

The Ark protects the pods through scripted arena changes.

The player does not perform an escort or pod-defense objective.

After Anti-Irys falls, the planetary-AI blade attempts to escape.

Irys prevents the escape.

Ion kills the planetary AI directly.

## Presentation

The game uses pixel-art presentation with modular rigged construction.

The visual identity depends on contrast between dark industrial restraint and bright saturated cyberpunk color.

Irys bodies are manufactured, synthetic, adult, feminine, attractive, variable, and cosplayable.

Ion has one persistent iconic blade design.

Anti-Irys has one fixed polished ideal-substrate design.

Gameplay framing favors zoomed-out readability.

Every boss uses a fixed authored camera zoom.

The ordinary HUD remains visible at all times.

The final battle transforms the recurring musical hook into a predominantly orchestral treatment.

---

# Explicitly Removed Legacy Concepts

The following are not current canon and must not be reintroduced without an explicit new design decision:

- a dynamic Synchronization combat meter;
- growing synchronization as a permanent upgrade system;
- Irys as the independent personality of each disposable run body;
- procedural room assembly;
- a futuristic city of rooftops, streets, arcologies, and transit districts as the principal campaign;
- card drafts;
- perk trees;
- equipment progression;
- currencies;
- permanent stat upgrades;
- body unlock progression;
- generic rewards after ordinary encounters;
- dash as a separate foundational movement ability;
- teleport as a separate foundational movement ability;
- wall interaction as a required foundational movement state;
- projectile reflection as a required Technique property;
- climbing structural seams as a required Technique property;
- elites or minibosses as a separate mandatory enemy tier;
- an adaptive procedurally changing final boss;
- player-centered hit-stop during ordinary successful Technique contact;
- the Creator as established current lore;
- Ion leaving the Creator as established current lore;
- unrestricted procedural stages;
- dynamically generated boss routes.

Do not infer these systems from old terminology, notebook material, commit history, or previous chat sessions.

---

# Current Work

The immediate active section remains:

`docs/audio/012 Audio Direction.md`

The next unresolved audio decision is:

Should Ion and Irys have clearly different voice signatures from the beginning, or should their tones share an obvious underlying sonic relationship that foreshadows their eventual merger?

After Audio Direction is complete:

1. close remaining Art Direction gaps;
2. approve Art Direction and Audio Direction;
3. define `docs/technical/010 Technical Specification.md`;
4. assemble `docs/999 Sol Implementation Specification.md`;
5. begin the one-shot implementation experiment.

Do not expand the documentation beyond what is necessary to reach that implementation.

---

# Definition of Ready

The project is ready for Sol when:

- all owning specifications are internally consistent;
- Technical Specification defines a feasible implementation architecture;
- Sol Implementation Specification integrates the required game into one executable plan;
- remaining uncertainty consists primarily of tuning, balancing, debugging, asset refinement, and polish;
- no major creative decision is required during implementation.

At that point, stop expanding the design and run the implementation experiment.

---

# Things To Avoid

Do not:

- reopen settled decisions without identifying a contradiction;
- treat obsolete foundational text as canon;
- invent unsupported lore;
- add conventional roguelite systems;
- add movement abilities because similar games contain them;
- duplicate mechanics across documents;
- create infrastructure instead of advancing the game;
- optimize for feature count;
- substitute statistical complexity for combat depth;
- use procedural generation to compensate for weak authored design;
- treat chat history as more authoritative than the repository;
- continue specification work after the project is implementation-ready.
