# Irys+Ion — Combat Model

**Version:** 2.0  
**Status:** Approved  
**Last Updated:** 2026-07-15  
**Owner:** Creative Direction

---

# Purpose

This document defines the conceptual architecture of combat in Irys+Ion.

It defines what combat is and how its major concepts relate.

It does not own:

- input mappings;
- exact Technique grammar;
- timings;
- animation behavior;
- collision formulas;
- damage values;
- enemy move lists;
- boss phases;
- progression formulas.

Those details belong to later owning specifications.

---

# Combat Philosophy

Combat is traversal under pressure.

Traversal and combat are not separate modes.

Ordinary movement establishes position.

The Language converts position and intention into Technique movement.

Every Technique changes:

- location;
- facing context;
- enemy relationships;
- available routes;
- immediate risk;
- the next useful decision.

Combat should feel like continuous route construction through danger.

The player should rarely experience a successful action as an isolated stationary attack.

---

# Primary Combat Entities

## Ion

Ion is the player.

Ion determines combat intention.

The player speaks The Language through Ion.

Ion persists across every run and every Irys body.

## Current Irys Substrate

The current Irys substrate is Ion’s physical wielder.

It executes Technique movement and receives damage.

Its generated properties affect:

- ordinary movement;
- structural integrity;
- reach;
- displacement resistance;
- hit recovery;
- Technique distance;
- Technique speed required to preserve duration;
- strike force;
- Prime hold duration;
- other approved body-dependent properties.

The body may change during the run.

The complete Language remains available.

The current body does not represent a dynamic synchronization relationship or persistent companion progression.

## Irys / Ark

Irys is the intelligence operating the Ark.

Irys is not a dynamic combat resource.

During the final duel, the Ark participates through scripted environmental protection and containment.

## Enemy

An enemy is an authored combat problem.

Each enemy tests one or more parts of The Language.

Enemy behavior must be observable and learnable.

## Arena

The arena is an active positional system.

Terrain, elevation, gaps, walls, ceilings, lanes, and moving structures shape combat decisions.

Solid terrain remains authoritative.

## Encounter

An encounter is an authored arrangement of enemies and arena conditions.

Its purpose is to create a readable problem that can be solved with increasing elegance.

Ordinary encounters remain low-density.

Boss encounters are the campaign’s primary combat examinations.

---

# Combat Resources and Persistent State

## Structural Integrity

Structural integrity is the current Irys body’s health.

When structural integrity reaches zero:

- the body is defeated;
- Ion rises from it;
- the run ends;
- Ion returns to the Ark;
- the next run begins with new candidate selection.

Structural integrity persists across the complete run.

Boss victories do not automatically restore it.

A replacement substrate has its own current and maximum structural integrity.

## Prime

Prime is not a consumable resource.

Prime is an armed combat state representing the intention to speak the next Technique in its Primed form.

Prime is:

- temporary;
- consumed by the next Technique;
- cleared by interruption;
- allowed to expire;
- retained through ordinary movement until one of those events occurs.

## No Dynamic Synchronization Resource

The combat model does not contain a dynamic Synchronization gauge, meter, combo state, or performance resource.

A generated body may possess a named compatibility or synchronization-related trait if defined by the Progression Specification.

That trait is a body property.

It is not a combat meter that rises and falls through play.

## No General Ability Resource Layer

The base combat model does not require:

- stamina;
- mana;
- Technique cooldowns;
- dash charges;
- teleport charges;
- combo meter;
- style meter;
- super meter.

Difficulty and commitment emerge from position, timing, enemy behavior, Technique authority, and the cost of surrendering flexibility.

---

# Combat Authority

At any moment, one system has primary authority over Irys’s movement.

The principal authority categories are:

1. Ordinary movement.
2. Normal Technique.
3. Primed Technique.
4. Authored hit or environmental reaction.
5. Defeat state.

Authority prevents contradictory systems from moving Irys simultaneously.

Detailed transfer rules belong to the Player Feel Specification.

---

# Ordinary Movement

Ordinary movement positions Irys while no Technique or authored reaction has authority.

Ordinary movement:

- does not override an active Technique;
- does not steer a Technique;
- does not consume Prime;
- remains subject to enemy body collision;
- remains subject to solid terrain and gravity.

The base conceptual model does not require separate foundational states for dash, teleport, or wall interaction.

---

# Normal Techniques

Advance, Ascend, and Descend have Normal expressions.

Normal Techniques represent fluid intention.

Conceptually:

- they answer immediately;
- they control movement while active;
- a new Normal Technique may replace the current Normal Technique;
- the new Technique begins from the current position;
- the outgoing Technique’s unfinished intention is abandoned;
- there is no mandatory mechanical recovery;
- successful contact does not stop the player’s movement;
- solid terrain may terminate the Technique.

Normal Techniques create continuous player-authored trajectories.

---

# Primed Techniques

Prime converts the next Advance, Ascend, or Descend into its Primed expression.

Primed Techniques represent committed intention.

Conceptually:

- they are distinct authored expressions;
- they are not merely damage-enhanced Normals;
- they retain authority through their defined execution;
- they may include committed recovery;
- ordinary movement does not steer them;
- new Technique inputs do not normally replace them;
- they provide strong interruption resistance without inherent invulnerability.

A Primed Technique may create a powerful positional or interruption result, but its primary cost is surrendered flexibility.

---

# Input Retention

There is no general Technique queue or input buffer.

A Technique input either resolves when entered or is not retained.

Prime is the sole retained combat intention because it is an explicit armed state rather than a queued Technique.

Inputs entered while control is unavailable do not automatically execute later.

---

# Contact Model

Technique damage is driven by eligible contact.

A Technique activation may hit a target once unless explicitly authored as multi-hit.

Enemy contact does not normally obstruct Technique movement.

Enemy bodies block ordinary movement.

Technique paths pass through enemies according to the Player Feel Specification.

Successful Normal Technique contact does not:

- pause Irys;
- slow Irys;
- stop Irys;
- recoil Irys;
- create player-centered hit-stop;
- seize control.

Impact is communicated primarily through:

- enemy response;
- sound;
- particles;
- temporary visual treatment;
- screen response;
- displacement where appropriate.

---

# Interruption Model

Enemy attacks may interrupt Irys.

During a Normal Technique, a valid interrupting hit:

- ends the active Technique;
- clears armed Prime;
- clears transient Technique state;
- places Irys into an authored hit reaction.

Inputs during the hit reaction are not retained.

Primed Techniques resist ordinary interruption but do not prevent damage.

Exceptionally forceful authored attacks may break Primed commitment.

Interruptibility must remain explicit and readable.

The project does not require a hidden universal stagger hierarchy.

---

# Terrain Model

Solid terrain remains authoritative.

A Normal Technique contacting solid terrain ends according to the Player Feel Specification.

A Primed Technique contacting solid terrain enters its authored terrain response.

Techniques do not pass through walls, floors, or ceilings unless a later owning specification explicitly creates such an exception.

The arena must remain dependable enough that the player can plan routes at high speed.

---

# Enemy Model

Enemies are authored state machines with readable behavior.

The three conceptual lineages are:

- Striker;
- Pursuer;
- Anchor.

Each lineage develops across three visible generations.

Later generations increase complexity and fluency without changing the fundamental combat grammar.

Enemy threat comes from:

- positioning;
- timing;
- attack commitment;
- spacing control;
- interruption;
- encounter combinations;
- arena context.

Enemy threat should not depend primarily on inflated health, hidden modifiers, or procedural behavior mutation.

---

# Boss Model

Bosses are authored examinations of The Language.

Each boss uses:

- fixed health;
- fixed attacks;
- fixed phase structure;
- authored vulnerability;
- clear telegraphs;
- contextual interruption rules;
- one persistent health bar.

Bosses do not statistically scale to the current Irys body.

The player’s generated body changes the conditions under which the fixed problem is solved.

The final Anti-Irys duel is the highest expression of the same model.

Anti-Irys may possess boss-authored combinations and values, but must obey the fundamental distinction between:

- responsive Normal Techniques;
- committed Primed Techniques.

---

# Combat Flow

The conceptual combat rhythm is:

Observe.  
Position.  
Choose.  
Express.  
Commit or redirect.  
Read the result.  
Continue.

This rhythm is not a scripted sequence.

It is the repeated structure of fluent decision-making.

The player should increasingly compress these steps until combat appears instinctive.

---

# Flow

Flow is not a resource.

Flow is an emergent result of:

- clear intention;
- immediate response;
- learned enemy behavior;
- dependable terrain;
- purposeful movement;
- successful redirection;
- accepted commitment;
- continuous decision-making.

Do not create a numerical Flow or Synchronization system to simulate what should emerge naturally from good play.

---

# Failure

Failure should normally result from:

- poor positioning;
- incorrect Technique selection;
- mistimed redirection;
- careless commitment;
- failure to read an enemy;
- entering a boss with insufficient remaining structural integrity;
- choosing a body poorly suited to the player’s approach;
- losing composure.

Failure should rarely result from:

- hidden rules;
- random untelegraphed behavior;
- inconsistent collision;
- retained inputs the player did not intend;
- arbitrary statistical scaling;
- impossible generated bodies;
- unavoidable damage.

---

# Success

Combat success is not defined only by reducing enemy health to zero.

The highest expression of success includes:

- purposeful positioning;
- correct Technique choice;
- clear use of redirection;
- deliberate use of Prime;
- understanding enemy commitments;
- using the arena;
- preserving structural integrity;
- maintaining control of the encounter;
- making difficult execution appear effortless.

No style score or synchronization meter is required to validate that success.

---

# Relationship to Progression

Generated Irys bodies alter the expression of combat without replacing its grammar.

A body may change:

- movement speed;
- acceleration;
- jump power;
- air control;
- reach;
- durability;
- displacement resistance;
- recovery;
- Technique distance;
- strike force;
- Prime hold duration.

A body may not remove:

- Advance;
- Ascend;
- Descend;
- Prime.

Normal Technique durations remain governed by the Player Feel Specification rather than changing with body quality.

Body generation creates changing conditions.

Player mastery solves them.

---

# System Relationships

This document owns the conceptual combat architecture.

Downstream ownership:

- Combat Language Specification defines vocabulary and grammar.
- Player Feel Specification defines response, authority, timing, collision, and interruption behavior.
- Enemy Bible defines ordinary enemy implementations.
- Boss Bible defines boss implementations.
- Progression Specification defines generated body variation and attrition.
- World Bible defines the playable campaign spaces.
- Art Direction defines combat presentation.
- Audio Direction defines combat sound hierarchy.
- Technical Specification defines implementation architecture.

If a downstream specification reveals a genuine conceptual contradiction, revise this document explicitly.

Do not preserve obsolete abstractions merely because they appeared in an earlier version.

---

# Definition of Success

The Combat Model succeeds when:

- every combat mechanic can be explained through position, intention, authority, commitment, contact, interruption, terrain, and body expression;
- no dynamic synchronization resource is required;
- no unrelated movement system is required to make combat function;
- generated bodies alter conditions without breaking The Language;
- enemies and bosses test the same coherent grammar;
- implementation can proceed without inventing additional foundational combat systems.
