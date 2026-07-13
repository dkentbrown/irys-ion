# Irys+Ion — Combat Model

**Version:** 1.0  
**Status:** Approved  
**Last Updated:** 2026-07-13  
**Owner:** Creative Direction

---

# Purpose

This document defines the conceptual architecture of combat within **Irys+Ion**.

It intentionally avoids implementation details such as frame counts, damage values, animation timing, collision rules, or input mappings.

Its purpose is to define **what combat is**, not **how combat is implemented**.

Every combat-related system in the project derives from the concepts established here.

---

# Combat Philosophy

Combat is not separate from traversal.

Combat is traversal under pressure.

Likewise, traversal is combat between encounters.

Movement and combat are two expressions of the same underlying system.

Every combat interaction should naturally reposition the player.

Every movement decision should naturally affect combat.

The player should never feel as though they are switching between two different gameplay modes.

---

# Primary Combat Entities

Combat consists of relationships between the following entities.

## Ion

Ion is the player.

Ion makes every combat decision.

Ion issues every attack.

Ion determines combat intent.

---

## Irys

Irys is Ion's host.

She physically executes Ion's intent.

Her increasing synchronization with Ion allows the player's growing mastery to be expressed through increasingly fluid movement.

---

## Enemy

Enemies are mechanical problems.

Each enemy exists to test one or more elements of the combat language.

Enemies should be understood before they are defeated.

---

## Arena

The arena is an active participant.

Platforms, walls, elevation, hazards, and open space shape every encounter.

Combat should encourage using the arena rather than ignoring it.

---

## Encounter

An encounter is a self-contained combat problem.

Its objective is not merely survival.

Its objective is elegant execution.

---

# Combat Resources

## Health

Represents remaining combat capability.

Loss of all health ends the current run.

---

## Synchronization

Synchronization represents the quality of cooperation between Ion and Irys.

It is not energy.

It is not stamina.

It is not mana.

It is a measurement of harmony between host and weapon.

Successful combat naturally increases synchronization.

Mistakes reduce it.

Synchronization should primarily amplify expression rather than compensate for poor execution.

---

## Prime

Prime represents Ion arming the alternate expression of the next Technique.

Prime is not a combat resource.

Prime is not a damage increase.

Prime is not a charge attack.

Prime does not immediately execute an attack.

The next Technique executes in its Primed form.

Prime is consumed immediately after that Technique executes.

Its exact mechanics belong to the Combat Language Specification.

---

# Combat States

Combat continuously transitions between a small number of conceptual states.

## Exploration

Movement without active combat.

Traversal dominates.

---

## Engagement

The player has entered an encounter.

Threat assessment begins.

---

## Combat

Active decision-making against one or more enemies.

Movement, attacks, and positioning become inseparable.

---

## Resolution

The encounter has been solved.

Rewards may be granted.

Player flow should be preserved whenever practical.

---

## Transition

Movement toward the next encounter.

---

# Player States

The player may conceptually exist in one or more movement states.

- Grounded
- Airborne
- Wall Interaction
- Dash
- Teleport
- Technique Execution
- Recovery

These states exist to describe combat behavior.

Their implementation belongs elsewhere.

---

# Enemy States

Enemies generally progress through recognizable states.

- Idle
- Aware
- Attacking
- Defending
- Vulnerable
- Broken
- Recovering
- Destroyed

Enemy design should minimize hidden state.

Players should understand what an enemy is currently doing through observation.

---

# Combat Flow

Combat should naturally create the following rhythm.

Observe.

Move.

Choose.

Commit.

Execute.

React.

Continue.

This loop repeats continuously until the encounter is solved.

The player should rarely stop moving while progressing through this sequence.

---

# Flow

Flow is not a resource.

Flow is an emergent property.

Players experience flow when movement, combat, positioning, timing, and decision-making become continuous.

Game systems should encourage flow.

They should never artificially force it.

---

# Failure

Failure should almost always result from one of the following.

- incorrect positioning
- incorrect technique selection
- poor timing
- predictable behavior
- unnecessary hesitation

Failure should rarely result from hidden information, random chance, or inconsistent controls.

---

# Success

An encounter is considered successful when:

- correct techniques were selected;
- movement remained purposeful;
- synchronization was maintained or improved;
- enemy mechanics were understood rather than bypassed;
- momentum was preserved whenever possible.

The highest form of success is not merely surviving.

It is making combat appear effortless.

---

# Adaptation

Players should constantly adapt.

Enemies should encourage adaptation.

Bosses should demand adaptation.

The final boss should master adaptation.

No encounter should be solved indefinitely by repeating a single sequence without thought.

---

# System Relationships

The Combat Model defines concepts only.

Subsequent documents expand those concepts.

- Combat Language Specification defines how the player communicates with Ion.
- Player Feel Specification defines how combat physically feels.
- Enemy Bible defines how enemies interact with the model.
- Boss Bible defines how large encounters express the model.
- Progression Specification defines how the model evolves across a run.

No later document may contradict this model.

If a contradiction is discovered, this document must be revised before downstream documents are updated.

---

# Definition of Success

This model succeeds when every combat mechanic introduced later can be explained using the concepts established here without introducing additional foundational ideas.

A developer reading this document should understand the architecture of combat before seeing a single input mapping, animation, or line of code.
