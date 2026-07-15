# Irys+Ion — Combat Language Specification

**Version:** 2.0  
**Status:** Approved  
**Last Updated:** 2026-07-15  
**Owner:** Creative Direction

---

# Purpose

This document specifies The Language, the primary combat vocabulary of Irys+Ion.

It defines:

- the four words;
- their meanings;
- input interpretation;
- Normal and Primed expression;
- Prime lifecycle;
- Technique sequencing;
- governing design laws.

The Player Feel Specification owns exact authority transfer, timing, motion, contact, collision, interruption, recovery, and hit-registration behavior.

---

# Philosophy

The Language is intentionally small.

Its depth comes from contextual expression rather than a large move list.

Every Technique represents:

- a direction;
- a positional intention;
- a combat decision;
- an opportunity for the next decision.

The player should eventually think in words and routes rather than buttons and isolated attacks.

---

# Core Vocabulary

The Language contains four foundational words.

| Combat input | Word | Core intention |
|---|---|---|
| Horizontal input matching current facing | Advance | Move decisively forward through the combat line. |
| Up | Ascend | Convert the current position into upward movement and vertical control. |
| Down | Descend | Drive downward and resolve elevation through committed vertical direction. |
| Horizontal input opposite current facing | Prime | Arm the alternate committed expression of the next Technique. |

Advance, Ascend, and Descend are Techniques.

Prime is an armed state.

---

# Input Relationship

Ordinary movement uses WASD.

Combat Language input uses the arrow keys.

The horizontal arrow matching Irys’s current facing speaks Advance.

The opposite horizontal arrow speaks Prime.

The up arrow speaks Ascend.

The down arrow speaks Descend.

Horizontal combat inputs are relative to facing rather than permanently bound to Advance or Prime.

If Irys is stationary, the current stored facing determines the relationship.

Ordinary movement and The Language are distinct input channels:

- ordinary movement positions the player when it has authority;
- The Language expresses combat intention.

When a Technique takes authority, ordinary movement does not steer or override it.

---

# Governing Laws

## The Language Always Answers

Every valid Technique input produces the selected Technique when control is available.

The system does not substitute a safer, more contextually appropriate, or differently directed action.

The player owns tactical mistakes.

## Player Intention Is Always Respected

The current action must reflect the player’s current valid intention.

Normal Techniques allow immediate new intention.

Primed Techniques preserve the player’s earlier decision to commit.

## Commitment Is Intention

A committed action is not unresponsive merely because the player cannot cancel it.

The inability to redirect a Primed Technique is the consequence of deliberately speaking Prime.

## Techniques Are Scale Invariant

Enemy size does not change the meaning of a Technique.

A large enemy may change:

- required clearance;
- contact response;
- interruption result;
- strategic use.

It does not change the grammar.

## Combat Is Positional

A Technique is not merely an animation that deals damage.

It is a positional decision that may produce damage through contact.

## Every Technique Leaves Opportunity

A completed Technique should place the player into another understandable combat state.

It must not terminate flow through arbitrary delay or loss of control unrelated to the chosen intention.

## The Full Language Is Universal

Every generated Irys body can speak:

- Advance;
- Ascend;
- Descend;
- Prime.

Body variation changes expression.

It does not remove vocabulary.

---

# Advance

Advance expresses decisive forward movement.

Its core purposes are:

- crossing the current combat line;
- moving through an opening;
- changing sides relative to an enemy;
- continuing a route;
- converting forward intention into contact.

Advance follows the current facing direction established at activation.

A successful path normally leaves Irys beyond a contacted enemy rather than stopped against it.

Advance is not:

- a stationary horizontal slash;
- an ordinary dash system separate from combat;
- a teleport;
- an automatic target selection system;
- a guarantee of safety.

Advance may be redirected by another Normal Technique according to the Player Feel Specification.

---

# Ascend

Ascend expresses upward Technique movement.

Its core purposes are:

- gaining elevation;
- attacking through an upward path;
- intercepting a vertically relevant threat;
- continuing combat into the air;
- converting the current route into vertical advantage.

Ascend always answers with upward Technique direction.

Ascend is not:

- an ordinary jump;
- an upward teleport;
- a requirement to reflect projectiles;
- a requirement to climb special structural seams;
- a context-sensitive substitute for an unrelated action.

Its exact motion profile and contact behavior belong to the Player Feel Specification.

---

# Descend

Descend expresses immediate downward Technique movement.

Its core purposes are:

- converting elevation into downward pressure;
- attacking through a downward path;
- returning toward lower combat space;
- resolving an aerial route;
- creating a deliberate downward commitment or redirection.

Descend always answers with downward Technique direction.

Descend is not required to:

- pin every enemy;
- break a universal defense state;
- anchor against every knockback;
- create a separate ground-stabilization system.

Those effects may exist only when explicitly authored elsewhere.

Its exact motion profile and contact behavior belong to the Player Feel Specification.

---

# Prime

Prime arms the alternate expression of the next Technique.

Prime does not execute an attack.

After Prime is spoken:

- Ion enters the armed Prime state;
- ordinary movement remains available;
- the next Advance, Ascend, or Descend becomes Primed;
- the armed state is consumed when that Technique begins.

Prime is not:

- a damage buff;
- a cooldown;
- a mana cost;
- a super mode;
- a combo reward;
- a resource meter;
- a conventional hold-to-charge attack.

Prime changes the grammar of the next word from fluid expression to committed expression.

---

# Armed Prime Lifecycle

Prime remains armed until one of the following occurs:

1. The next Technique consumes it.
2. An enemy interruption clears it.
3. The hold duration expires.
4. Another explicitly authored state clears transient combat state.

Ordinary movement does not consume Prime.

Prime may remain armed while the player repositions.

The base target is a brief tunable hold long enough to permit intentional repositioning, approximately one to two seconds unless playtesting establishes a better value.

The world-space cue lives primarily on Ion.

It remains steady rather than visually decaying.

Prime expiration:

- occurs completely;
- creates a small readable cue;
- imposes no recovery;
- imposes no penalty;
- does not weaken gradually before expiration.

There is no countdown timer requirement.

---

# Normal Expression

Advance, Ascend, and Descend each have a Normal expression.

Normal Techniques are:

- fluid;
- immediate;
- responsive;
- chainable;
- redirectable.

A new Normal Technique may take authority from an active Normal Technique.

The incoming Technique begins from the current position.

The outgoing Technique does not force completion.

The same Normal Technique may be activated again as a fresh expression.

Normal Technique sequencing does not require:

- a combo timer;
- a sequence table;
- a neutral frame;
- a completed animation;
- target contact;
- a mandatory recovery;
- an input buffer.

The Player Feel Specification defines exact authority transfer and hit eligibility.

---

# Primed Expression

Advance, Ascend, and Descend each have a distinct Primed expression.

A Primed Technique is not merely the Normal Technique with:

- more damage;
- a larger effect;
- a longer distance;
- a universal armor property.

Its movement, timing, contact response, and recovery may be authored differently according to its intention.

Primed Techniques are:

- deliberate;
- committed;
- weighty;
- less flexible;
- contextually powerful.

Once a Primed Technique begins, it normally retains authority through:

- execution;
- contact response;
- follow-through;
- authored recovery.

Another Technique does not normally redirect it.

Ordinary movement does not steer it.

Inputs entered while control is unavailable are not retained for later execution.

Priming every Technique should naturally feel clumsy because the player repeatedly surrenders flexibility.

No stamina, cooldown, or artificial usage restriction is required.

---

# Normal and Primed Relationship

Normal and Primed Techniques are two grammatical expressions of the same word.

They share directional identity.

They differ in commitment.

Normal expression asks:

> What do I intend now?

Primed expression states:

> I choose to complete this intention.

Prime does not invalidate the underlying meaning of Advance, Ascend, or Descend.

It changes how decisively that meaning is expressed.

---

# Sentences

A Technique is a word.

A sequence of Techniques may be described as a Sentence.

Sentences are design vocabulary.

They are not an explicit tracked gameplay system.

The implementation does not require:

- combo counters;
- sentence meters;
- grammatical scoring;
- fixed recipes;
- sequence bonuses;
- named combo lists;
- rewards for sentence length.

A Sentence naturally continues while the player keeps expressing connected combat intentions.

It naturally concludes when:

- the player stops;
- ordinary movement resumes;
- combat circumstances end the route;
- an authored interruption occurs;
- a committed action resolves without continuation.

Longer and more elegant Sentences should emerge from player fluency.

---

# Input Retention

There is no general Technique queue or buffer.

Technique inputs resolve when entered and control is available.

Inputs entered while a Primed Technique, hit reaction, or other authored lockout owns control are ignored rather than stored.

Prime is the only retained combat intention.

Prime is not buffered Technique input.

It is a visible armed state.

---

# Interruption

During a Normal Technique, a valid enemy hit may:

- end the active Technique;
- clear armed Prime;
- clear transient Technique state;
- impose an authored hit reaction.

Inputs entered during that reaction are not retained.

Primed Techniques provide strong resistance to ordinary interruption.

They do not provide inherent invulnerability.

Exceptionally forceful authored attacks may break Primed commitment.

The exact response belongs to the Player Feel Specification and the attacking enemy or boss specification.

---

# Contact and Scale

Technique damage occurs through eligible contact.

Enemy bodies do not normally stop an active Technique.

The Technique continues along its authored or contact-adjusted path.

Advance, Ascend, and Descend preserve their fixed authored Normal duration even when enemy clearance requires additional travel.

The implementation may increase travel speed as required to preserve the duration.

A Technique may hit multiple different enemies.

One activation may hit a given target once unless explicitly authored as multi-hit.

Repeated activation while still overlapping the same target does not create immediate repeated damage.

Exact separation and hit-registration rules belong to the Player Feel Specification.

---

# Terrain

Solid terrain remains authoritative.

The Language does not imply passage through walls, floors, or ceilings.

A Normal Technique contacting terrain terminates according to the Player Feel Specification.

A Primed Technique contacting terrain enters its authored terrain response.

Terrain interaction must remain:

- readable;
- consistent;
- learnable;
- subordinate to the Technique’s directional meaning.

---

# Body Variation

Generated body properties may alter:

- Technique distance;
- required travel speed;
- attack reach;
- strike force;
- exit velocity;
- ordinary movement;
- displacement resistance;
- hit recovery;
- Prime hold duration.

Generated body properties do not alter:

- the existence of a word;
- the word’s direction;
- the distinction between Normal and Primed expression;
- the governing authority rules;
- fixed authored Normal Technique duration.

A weak body may express a Technique poorly.

The Technique must still answer.

---

# Acquisition of Fluency

Every Technique should feel satisfying in isolation.

A new player may produce entertaining movement before understanding the full system.

As encounters become more demanding, deliberate expression becomes increasingly valuable.

The player should not feel that the controls stopped working.

The player should recognize that a different intention, timing, route, or commitment was required.

The progression is:

button  
↓  
word  
↓  
route  
↓  
sentence  
↓  
fluency

Eventually the player should think directly in The Language.

---

# Implementation Requirements

The implementation must preserve:

- four-word vocabulary;
- relative horizontal Advance and Prime mapping;
- separate ordinary movement and combat inputs;
- immediate Normal Technique response;
- immediate Normal-to-Normal authority transfer;
- same-Technique Normal reactivation;
- committed Primed execution;
- no general Technique buffering;
- Prime as the sole retained combat intention;
- Prime persistence through ordinary movement;
- Prime expiration;
- no universal damage-buff interpretation of Prime;
- complete Language availability for every generated body;
- scale-invariant Technique meaning;
- enemy pass-through during Techniques;
- solid-terrain authority;
- fixed Normal Technique duration;
- contact-adjusted enemy clearance;
- per-activation hit eligibility;
- no player-centered hit-stop on ordinary successful contact;
- no inherent Technique invulnerability;
- readable interruption and recovery.

---

# Scope Boundaries

The Combat Language Specification does not require:

- additional combat words;
- diagonal Technique inputs;
- fighting-game motion inputs;
- light and heavy attack buttons;
- a conventional combo list;
- projectile reflection;
- structural-seam climbing;
- target locking;
- automatic context-sensitive Technique substitution;
- separate dash or teleport attacks;
- a synchronization meter;
- a style score;
- stamina;
- mana;
- Technique cooldowns;
- procedural Technique generation;
- body-specific loss of vocabulary;
- universal Primed damage bonuses;
- future sentence-scoring systems.

Additional Language systems should be introduced only when the current vocabulary demonstrably cannot support the implemented game.

---

# Design Laws

- The Language Always Answers.
- Player Intention Is Always Respected.
- Commitment Is Intention.
- Techniques Are Scale Invariant.
- Movement Is Combat.
- Combat Is Traversal Under Pressure.
- Every Technique Leaves Opportunity.
- The Full Language Is Universal.
- Normal Expression Is Fluid.
- Primed Expression Is Committed.
- Prime Is the Sole Retained Combat Intention.
- Player Mastery Is Canonical.
