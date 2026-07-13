# Irys+Ion — Combat Language Specification

**Version:** 1.0  
**Status:** Draft  
**Last Updated:** 2026-07-13  
**Owner:** Creative Direction

---

# Purpose

This document specifies **The Language**, the primary combat system of Irys+Ion.

Unlike conventional action games, the player does not issue attacks.

The player speaks **The Language** through motion.

Combat emerges naturally from the sequence of words spoken.

This document defines that language.

---

# Philosophy

The Language is intentionally small.

Its depth comes from contextual expression rather than a large move list.

Every Technique represents a motion.

Every motion represents an intention.

Enemies, bosses, and environments exist to challenge the player's fluency in speaking The Language.

---

# Core Vocabulary

The Language contains four foundational words.

| Input | Word | Purpose |
|-------|------|---------|
| → | Advance | Change sides through an opening. |
| ↑ | Ascend | Convert combat into elevation. |
| ↓ | Descend | Resolve position and prepare the next opportunity. |
| ← | Prime | Arm the alternate expression of the next Technique. |

### Relative Horizontal Inputs

Advance is always spoken using the horizontal combat input matching Irys' current facing direction.

Prime is always spoken using the opposite horizontal combat input.

These inputs are relative to facing rather than absolute screen direction.

If Irys is stationary, her current facing determines the mapping.

Movement (WASD) is independent from The Language.

Movement positions the player.

The Language expresses intent.

Together they produce combat.

---

# Fundamental Principles

## The Language Always Answers

Every valid Technique always produces a meaningful action.

The system never substitutes a different Technique.

The player owns tactical mistakes.

Ion faithfully expresses every chosen word.

---

## Techniques Are Scale Invariant

The meaning of a Technique never changes because an opponent is larger or smaller.

Enemy size changes the combat problem.

It never changes the grammar.

---

## Techniques Define Intent

Techniques define geometric intent and destination.

They do not define animation.

The implementation may choose different visual expressions provided the conceptual meaning of the Technique remains unchanged.

---

## Combat Is Positional

The Language is not a sequence of attacks.

It is a sequence of positional decisions that naturally produce combat.

---

# Technique Model

Every Technique consists of a conceptual intention rather than a fixed animation.

The player should think in words rather than attacks.

---

## Advance

Advance changes sides.

Ion identifies a valid path through an opening.

Irys follows that path.

Advance should preserve forward momentum whenever practical.

On successful execution, Advance normally leaves the player beyond the target.

Advance should never feel like striking a wall.

---

## Ascend

Ascend converts combat into elevation.

The Technique always produces upward movement.

Possible contextual expressions include:

- launching through enemies;
- climbing structural seams;
- reflecting projectiles upward;
- gaining aerial position;
- following weak points vertically.

Ascend is not an upward jump.

Ascend is not an upward teleport.

---

## Descend

Descend resolves position.

It converts elevation into certainty.

Possible contextual expressions include:

- driving through an opponent;
- pinning a target;
- anchoring against knockback;
- breaking defensive stances;
- stabilizing on the ground;
- preparing the next positional decision.

Experienced players will often—but not always—end a Sentence with Descend because it naturally restores opportunity.

Descend is punctuation rather than termination.

---

# Prime

Prime arms the alternate expression of the next Technique.

Prime does not immediately execute an attack.

After Prime is spoken, Ion enters an Armed state.

The next Technique executes in its Primed form.

Prime is then consumed.

If the player hesitates for too long before speaking the next Technique, Prime expires naturally and Ion returns to the Idle state.

Prime should communicate readiness through subtle visual and audio feedback.

No countdown timer should be presented to the player.

Primed Techniques are intentionally more committed than their normal counterparts.

They are not designed to increase player power.

They exist to create emphasis.

Normal Techniques flow naturally together into continuous movement.

Primed Techniques deliberately slow the rhythm of combat just enough to communicate intent, confidence, and commitment.

They should feel decisive without becoming sluggish.

Using Prime on every Technique should naturally feel cumbersome rather than strategically optimal.

The player should instinctively reserve Primed Techniques for moments that deserve emphasis.

Prime is not:

- a damage buff;
- a cooldown;
- a super mode;
- a resource meter;
- a charge attack.

Prime is the deliberate emphasis of the next spoken word.

---

# Sentences

A Technique is a word.

Sequences of Techniques form Sentences.

Sentences have no fixed length.

Sentences are currently a design vocabulary rather than an explicit gameplay resource.

The first implementation does not track, score, or reward Sentences directly.

They exist to describe how Techniques naturally chain together into expressive combat.

Future implementations may build additional systems on this concept without changing the underlying Combat Language.

They naturally conclude when:

- the player intentionally resolves the phrase;
- the player allows the current Technique to finish without continuing;
- combat flow is interrupted;
- circumstances naturally end the exchange.

Longer Sentences should emerge from player fluency rather than arbitrary combo systems.

---

# Acquisition of Fluency

Every Technique should feel satisfying in isolation.

New players should immediately enjoy movement and experimentation.

Button mashing should remain entertaining during early encounters.

As the game progresses, increasingly sophisticated enemy design naturally rewards deliberate expression.

The player should never feel that the controls stopped working.

Instead they should conclude:

> "I need a better sentence."

The player gradually stops thinking in buttons.

Eventually they begin thinking directly in The Language.

---

# Design Laws

The Language Always Answers.

Techniques Are Scale Invariant.

Movement Is Combat.

Combat Is Traversal Under Pressure.

Every Technique Leaves Opportunity.

Player Mastery Is Canonical.
