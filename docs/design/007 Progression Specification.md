# Progression Specification

Status: Approved

## Purpose

This document defines the complete roguelite and run-progression structure for Irys+Ion.

The progression system exists to create substantial variation between attempts without adding conventional permanent upgrades, card drafts, currencies, loot trees, or unlock grinds.

The generated Irys body is the primary roguelite system.

Player mastery of The Language and the fixed boss sequence is the primary long-term progression.

## Progression Principles

Every run begins fresh at the Ark.

Ion persists across runs.

Created Irys bodies do not retain memory, identity, equipment, or progression from previous attempts.

The full Irys generation pool is available from the first run.

No body traits, mutations, superior stat ranges, or anatomical possibilities are unlocked through metaprogression.

The game does not require:

- permanent stat upgrades;
- unlockable power tiers;
- card drafting;
- currencies;
- equipment inventories;
- consumable healing items;
- permanent boss skips;
- alternate starting floors;
- a separate perk-selection layer.

The player progresses through:

- learning The Language;
- learning the fixed bosses;
- evaluating generated Irys candidates;
- adapting play style to the selected body;
- deciding when to retain or replace a damaged substrate during the run.

## Run Structure

Every attempt follows the complete fixed sequence:

1. Ark candidate selection.
2. First outward stage and boss.
3. First post-boss substrate decision.
4. Second outward stage and boss.
5. Second post-boss substrate decision.
6. Third outward stage and boss.
7. Third post-boss substrate decision.
8. Control-center stage and mechanical boss.
9. Return to the Ark.
10. Final confrontation.

There are no permanent checkpoints or boss skips.

A run always begins at the Ark.

## Ark Candidate Generation

Before each run, the Ark generates three Irys candidates.

The player may inspect all three before making a selection.

The inspection interface must show:

- the visible body;
- anatomical variations;
- named traits;
- exact gameplay-relevant statistics;
- current and maximum structural integrity;
- meaningful strengths;
- meaningful disadvantages;
- major statistical outliers.

The player chooses one candidate.

The selection is final for the beginning of that run.

There is no unlimited rerolling.

There is no playable audition or testing room before selection.

The exact feel of the selected body is discovered during the run, but its important mechanical properties must be visible beforehand.

## Candidate Diversity

The three candidates are generated independently and then checked for minimum meaningful diversity.

The Ark should not present three functionally near-identical bodies.

At least one material difference should exist across the offered candidates in areas such as:

- anatomy;
- movement;
- durability;
- Technique geometry;
- Prime behavior;
- hit recovery;
- critical-hit profile;
- dominant side;
- play-style suitability.

The diversity check does not equalize power.

It does not guarantee that one candidate is strong, weak, rare, or balanced.

Its only purpose is to make the selection meaningful.

## Unequal Power

Generated Irys candidates are not balanced around an equal hidden power budget.

Some bodies may be:

- plainly weaker;
- average;
- highly specialized;
- broadly capable;
- unusually strong;
- aesthetically exceptional;
- mechanically overpowered;
- defective but still viable.

A strong body does not require a compensating weakness.

A defective body may simply make the run harder.

Some combinations should strongly favor particular play styles.

Rare combinations may produce memorable runs that feel broken or overpowered.

The requirement is broad viability, not mathematical equality.

Every generated Irys must remain capable of completing the game when used with sufficient player skill.

Every generated Irys must remain capable of expressing the complete Language.

## Rarity

The full generation pool is active from the first run.

Generation uses weighted rarity rather than unlock progression.

Most candidates should cluster around ordinary capability and appearance.

Exceptionally strong bodies should be uncommon.

Exceptionally striking bodies should be uncommon.

A body that is both mechanically exceptional and aesthetically exceptional should be particularly rare.

Mechanical quality and visual quality are not perfectly linked.

The system may generate:

- attractive but weak bodies;
- unusual-looking but powerful bodies;
- ordinary-looking exceptional bodies;
- visually striking defective bodies.

No overall rarity tier, power score, or quality grade needs to be shown.

The player evaluates the actual body, traits, and statistics.

## Generation Model

Each Irys combines:

- continuous numerical properties;
- discrete anatomical configurations;
- named traits;
- derived statistics.

The system should generate combinations rather than select from a small pool of complete fixed archetypes.

The body’s visible anatomy should correspond to its mechanical properties.

Statistics should be constrained by the visible structural build rather than rolled independently without consequence.

Examples:

- greater mass may reduce acceleration while increasing maximum structural integrity;
- longer limbs may increase reach while affecting balance;
- a SingleLeg wheel-foot monopod may reduce ordinary movement flexibility while leaving The Language functional;
- an AdditionalArm body may alter reach, stability, or strike force;
- precision construction may improve critical-hit chance or multiplier;
- unstable joints may improve flexibility while worsening recovery or control;
- a reinforced torso may improve integrity without requiring an unrelated drawback.

The implementation does not require a generalized biological simulation.

The generation system should remain data-driven and bounded.

## Surfaced Irys Properties

Each candidate should expose the following generated gameplay properties:

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

Universal across all frames:

- damage resistance;
- enemy-authored knockback;
- gravity and base falling behavior;
- Normal Technique duration;
- Language vocabulary;
- core interruption rules;
- terrain collision rules;
- hit eligibility rules.

These properties are implementation targets rather than immutable labels.

A property may be renamed, merged, or derived if implementation reveals redundancy, provided the generation system still creates substantial visible and mechanical variation.

## Authored Anatomy Configurations

The first implementation uses exactly four authored body configurations:

1. Standard
2. SingleArm
3. SingleLeg
4. AdditionalArm

Reduced-limb configurations are intentional Ark specializations rather than failed assembly.

Every configuration must remain complete, viable, and capable of expressing the full Language.

SingleArm uses purpose-built sealed anatomy and may retain either arm.

Any valid arm may wield Ion.

SingleLeg is a purpose-built wheel-foot monopod with a small rear stabilization and thrust unit.

The wheel-foot and thruster support locomotion, braking, jumping, stabilization, and Technique presentation without providing continuous hovering.

AdditionalArm uses one third arm attached at a shoulder.

Any of its valid arms may wield Ion.

The AdditionalArm configuration may automatically transfer Ion between arms during authored motion.

This transfer creates no new inputs, dual wielding, extra Technique vocabulary, or additional hit events.

Every generated body also has a dominant side or handedness.

Dominant side may influence small surfaced differences in handling, strike expression, recovery, or control while preserving viability.

## Collision Profiles

Generated frames use three bounded gameplay collision profiles:

- small;
- standard;
- large.

The profile is derived from coherent body construction.

Visual anatomy may vary more broadly than the gameplay hurtbox.

Do not generate arbitrary per-body collision meshes.

## Named Traits

Discrete anatomical or structural conditions should appear as named traits.

Examples include:

- SingleArm
- SingleLeg Wheel-Foot
- AdditionalArm
- Elongated Reach
- Reinforced Torso
- Unstable Joints
- Asymmetric Frame
- Dense Structure
- Lightweight Frame
- Reduced Mobility
- High Synchronization

Named traits should summarize meaningful body conditions.

They should not replace the exact statistics shown in the inspection interface.

The interface should explain their practical effects in plain language.

## Relationship to The Language

Every generated Irys must be able to use:

- Advance;
- Ascend;
- Descend;
- Prime.

Body variation may change how Techniques perform.

It must not change whether The Language answers.

Normal Technique durations remain fixed according to the Player Feel Specification.

Body variation may affect:

- Technique distance;
- required travel speed;
- attack reach;
- strike force;
- critical-hit chance;
- critical-hit multiplier;
- exit velocity;
- ordinary movement;
- hit recovery;
- Prime hold duration.

Hit-reaction recovery varies according to coherent body construction.

Damage resistance and enemy-authored knockback are universal across all generated frames.

A weak or defective body may perform a Technique poorly.

The Technique must still execute.

## Health and Attrition

Structural integrity persists across the entire run.

Defeating a boss does not automatically restore the current Irys to full health.

Damage taken during earlier encounters remains relevant.

A durable Irys therefore has value across the complete gauntlet.

A fragile but powerful Irys creates a high-risk run.

The game does not require a conventional healing-item system.

Recovery and build changes occur through optional substrate transfer after the first three bosses.

## Post-Boss Substrate Selection

After defeating each of the first three outward bosses, Ion may inspect viable planetary-AI-produced Irys substrates near or within the boss area.

The player may choose to:

- remain in the current Irys with her current remaining integrity and build;
- transfer Ion into one of the available substrates.

Each available substrate has its own:

- anatomy;
- traits;
- statistics;
- maximum integrity;
- remaining integrity;
- visual appearance;
- play-style implications.

Transfer replaces the complete physical build.

No anatomy, statistics, damage state, or body-specific trait carries into the new substrate.

Ion, player knowledge, and narrative state persist.

Prime and all other transient combat states clear during transfer.

The abandoned body remains behind.

The route does not require returning to retrieve it.

## Post-Boss Selection Count

Each post-boss selection should present two or three viable substrates.

The player may inspect them before choosing.

Remaining in the current body must always remain an option.

The game should never force a transfer solely because a selection room has been reached.

## Post-Boss Environmental Progression

The substrate-selection spaces should reflect the planetary AI’s increasing success at imitating Irys.

### After the First Boss

The available bodies appear crude and inconsistently stored.

A small attached chamber may contain viable substrates arranged, suspended, or piled against walls.

Their anatomy and statistics may trend rougher or more defective.

### After the Second Boss

The available bodies appear more deliberately fabricated and organized.

Storage, suspension, or manufacturing systems should be more refined.

The bodies may show greater specialization and a modest increase in average quality.

### After the Third Boss

The available bodies appear in pod-like systems resembling the Ark.

This visually confirms that the planetary AI is approaching Irys’s production method.

This final normal selection has the highest probability of presenting an exceptional substrate.

It does not guarantee one.

### After the Control-Center Boss

There is no normal substrate-selection room.

The Ark crisis and return sequence begin immediately.

## Later-Selection Quality Trend

Later post-boss selections should trend slightly upward in average quality.

The final normal selection may have higher odds of generating unusually strong or aesthetically exceptional bodies.

This progression is probabilistic rather than guaranteed.

A later body is not automatically better than an earlier body.

A rare Ark-generated starting Irys may remain superior to every substrate presented during the run.

The starting Irys must always remain capable of clearing the complete game.

A sufficiently skilled player may complete the entire run without transferring.

## Transfer Decisions

Post-boss transfer should create meaningful choices such as:

- retain a powerful but badly damaged body;
- switch into a healthier but weaker body;
- abandon a specialized play style for a more durable build;
- replace a poor starting body with a superior experiment;
- refuse a superficially stronger body because it conflicts with the player’s preferred use of The Language.

The system should not reduce these decisions to automatic healing.

Remaining HP is only one part of the comparison.

## No Separate Upgrade Layer

The Irys body system is the complete roguelite progression layer unless later implementation reveals a clear blocking need.

Do not add:

- between-boss card drafts;
- passive item inventories;
- permanent currencies;
- stat-purchase menus;
- weapon rarity;
- equipment drops;
- conventional talent trees;
- permanent body unlocks;
- consumable healing stockpiles.

Additional progression systems should be introduced only if their absence clearly prevents the game from functioning.

## Implementation Requirements

The implemented progression system must include:

- three Ark candidates before every run;
- inspection of visible anatomy, traits, and exact statistics;
- no unlimited rerolling;
- no playable audition before selection;
- minimum meaningful diversity among the three candidates;
- the surfaced generated gameplay properties defined in this specification;
- both continuous statistics and discrete traits;
- visible anatomical variation;
- exactly four authored anatomy configurations;
- Standard, SingleArm, SingleLeg, and AdditionalArm support;
- dominant side or handedness;
- critical-hit chance and critical-hit multiplier;
- three bounded collision profiles;
- universal damage resistance and enemy-authored knockback;
- unequal candidate power;
- weighted rarity;
- the full generation pool from the first run;
- rare exceptional bodies;
- complete Language availability for every Irys;
- persistent health across the run;
- no automatic full healing after bosses;
- two or three optional substrates after each of the first three bosses;
- retention of the current damaged body as an option;
- complete body replacement when transferring;
- increasingly refined planetary-AI substrate rooms;
- a slight upward trend in later selection quality;
- no guaranteed upgrade;
- no normal selection after the control-center boss;
- no permanent metaprogression;
- no separate card, loot, currency, or equipment system.

## Scope Boundaries

The Progression Specification does not require:

- procedural animation generation;
- generalized anatomical simulation;
- hundreds of unique named traits;
- equalized candidate balance;
- guaranteed tradeoffs;
- permanent unlock trees;
- alternate campaign routes;
- dynamic boss generation;
- procedural stages;
- persistent equipment;
- multiplayer body comparison;
- a complete statistical balance pass before implementation.

Initial values, rarity weights, and derived-stat formulas should be data-driven and tuned through playtesting.
