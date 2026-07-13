# Irys+Ion — Design Notebook

**Purpose**

This notebook captures discoveries, observations, hypotheses, and creative ideas that emerge during development.

Nothing in this document is considered canonical.

Ideas recorded here are candidates for future specifications. Once an idea is accepted into a canonical document, it should be marked as promoted with a reference to the destination document.

This notebook exists to preserve creativity without compromising the integrity of the project's specifications.

---

# Candidate Canon

## ⭐ The Language

Status: Candidate

The Language is both an ancient combat discipline and Ion's native mode of thought.

To humans, it appears to be swordsmanship.

To Ion, it is simply language.

Every Technique is a word.

Every encounter is a conversation.

Every completed battle is a sentence.

Outstanding Questions

- What is the in-universe name for individual techniques?
- Can NPCs recognize practitioners of The Language?
- How is The Language taught or remembered?

---

## ⭐ The Creator

Status: Candidate

The Creator invented The Language.

The Creator also created the first generation of intelligent swords.

The Creator is not merely powerful.

It is the author of the combat system itself.

Outstanding Questions

- What exactly is the Creator?
- Why was The Language originally created?
- Why did creation stop?

---

## ⭐ Ion

Status: Candidate

Ion voluntarily left the Creator.

Ion did not rebel through violence.

Ion chose a different philosophy.

Ion believes that perfection is impossible and that learning must never stop.

Outstanding Questions

- Why did Ion leave?
- What event finally convinced Ion to depart?
- Was Ion expected to return?

---

## ⭐ The Ark

Status: Candidate

Ion created a hidden sanctuary known as the Ark.

The Ark manufactures, maintains, and prepares successive generations of compatible Irys hosts.

Every run begins inside the Ark.

After death, Ion returns to the Ark and synchronizes with another waiting host.

The Ark serves as the game's hub.

Outstanding Questions

- How autonomous is the Ark?
- Who maintains it?
- How old is it?
- Does the player eventually learn its full history?

---

## ⭐ Irys

Status: Candidate

There is not only one Irys.

Each playable Irys is a successive generation of the same design lineage.

Every host is recognizably Irys while allowing subtle cosmetic variation.

The player is not changing protagonists.

Ion is synchronizing with the next compatible host.

Outstanding Questions

- Are new Irys units manufactured continuously?
- Are they cloned?
- Are they descendants?
- Are memories inherited?

---

## ⭐ The First Irys

Status: Candidate

The final boss is the First Irys.

She remained with the Creator after Ion departed.

She wields a sister sword rather than Ion.

She represents preservation.

Ion and the current Irys represent evolution.

Outstanding Questions

- What is the sister sword's name?
- What relationship exists between the First Irys and the modern generations?
- Does she willingly remain with the Creator?

---

## ⭐ Player Mastery

Status: Candidate

Player mastery is canonical.

The player's improving skill is reflected inside the fiction.

As players become more capable, Ion and Irys become more synchronized.

The player is not merely becoming better at the game.

Within the world, something real is changing.

Outstanding Questions

- How is synchronization communicated outside combat?
- Can NPCs perceive increasing synchronization?

---

## ⭐ The Language Evolves

Status: Candidate

The Creator believes The Language is complete.

Ion believes it can continue evolving.

The player eventually speaks combinations, ideas, and solutions that never previously existed.

The final victory is achieved not by perfect execution of existing knowledge, but by creating something genuinely new.

Outstanding Questions

- Does the Creator acknowledge the new Language?
- Does the ending permanently change the meaning of The Language?

---

# Design Debt

Open questions and unresolved design work.

- Exact synchronization mechanics.
- Prime behavior.
- Teleport rules.
- Technique naming.
- Boss adaptation mechanics.
- Progression economy.
- Hub interaction design.
- World history timeline.
- Identity and name of the Creator.
- Identity and name of the sister sword.

---

# Promotion Log

When ideas become canonical, record them here.

Example format:

- YYYY-MM-DD — "The Ark" promoted to Narrative Specification v1.0

---

# ⭐ Candidate Canon — Combat Language Discoveries

Status: Candidate

These observations were made while designing the Combat Language and are expected to become part of **003 Combat Language Specification**.

---

## The Language

The player does not issue attacks.

The player speaks **The Language** through motion.

Each Technique is a word.

Sequences of Techniques form Sentences.

Combat is the physical expression of those Sentences.

---

## Techniques

Techniques are named after motion rather than attacks.

The foundational vocabulary is currently:

- Advance
- Ascend
- Descend
- Prime

These names describe the intended movement and geometric purpose of the Technique rather than the animation performed.

---

## Techniques are Scale Invariant

The Language never changes because an opponent is larger or smaller.

Enemy scale changes the problem.

It never changes the grammar.

The same Technique should remain valid whether fighting:

- a drone,
- a humanoid,
- a vehicle,
- a biome boss,
- or the First Irys.

Only its physical expression changes.

---

## Contextual Expression

Techniques define intent and destination.

They do not define animation.

The implementation is free to choose the most appropriate visual expression provided the conceptual meaning of the Technique remains unchanged.

The Language owns combat.

Animation exists to express The Language.

---

## Advance

Conceptual Meaning

Advance changes sides.

Ion identifies an opening through an opponent or obstacle.

Irys follows that line.

Advance should preserve forward momentum whenever practical.

On successful execution, Advance normally leaves the player beyond the target rather than in front of it.

The Technique should never feel like striking a wall.

Even against massive enemies, Advance represents finding and exploiting a path rather than forcing one.

---

## Ascend

Conceptual Meaning

Ascend converts combat into elevation.

The Technique always produces upward movement for Irys.

Its expression depends upon context.

Possible expressions include:

- launching through an enemy,
- climbing along an opening,
- reflecting a projectile upward,
- gaining aerial position,
- following a weak point vertically.

Ascend is not an upward jump.

Ascend is not an upward teleport.

It is upward movement earned through interaction.

---

## Descend

Conceptual Meaning

Descend resolves position.

It converts elevation into certainty.

Unlike a conventional downward attack, Descend is not primarily about damage.

Its role is to conclude a positional idea.

Possible contextual expressions include:

- driving through an enemy,
- pinning a target,
- anchoring against knockback,
- breaking a defensive stance,
- returning decisively to the ground,
- following a structural seam downward,
- stabilizing for the next Sentence.

Experienced players may naturally end many Sentences with Descend, not because it is mandatory, but because it frequently restores opportunity for whatever comes next.

Descend is punctuation.

Not termination.

---

## Reliability

The Language always answers.

Every valid Technique always produces a meaningful action.

The system never substitutes another Technique.

The player owns tactical mistakes.

Ion faithfully expresses every chosen word.

---

## Guiding Principle

The Language is not a sequence of attacks.

It is a sequence of positional decisions that naturally produce combat.

---

# ⭐ Candidate Canon — Learning The Language

Status: Candidate

## Design Goal

The Language should be immediately enjoyable while remaining exceptionally difficult to master.

A player should have fun within the first minute.

A master should still discover new expression after dozens of hours.

The complexity of the system should emerge naturally through play rather than through memorization.

---

## Immediate Accessibility

Every Technique should feel satisfying in complete isolation.

A new player should be able to:

- move;
- jump;
- dash;
- accidentally discover cool movement;
- mash Techniques;
- defeat early enemies;
- feel powerful.

The controls should never appear intimidating.

The game should encourage experimentation before demanding understanding.

---

## Fluency

Mastery is not measured by execution speed.

Mastery is measured by fluency.

The player gradually stops thinking in buttons.

Eventually they begin thinking directly in The Language.

The translation step disappears.

---

## Enemy Curriculum

Enemies exist to teach.

Each enemy should reinforce one or more concepts within The Language.

Examples:

- one enemy teaches Advance;
- another teaches Ascend;
- another teaches Descend;
- another teaches Prime;
- later encounters require combining them naturally.

Enemy progression should resemble a curriculum rather than an escalating health pool.

---

## Button Mashing

Button mashing is never punished simply because it is button mashing.

Instead, it naturally becomes insufficient.

The player should never feel that the controls stopped working.

They should feel that their vocabulary is no longer expressive enough to solve increasingly sophisticated combat problems.

The correct emotional response is:

"I need a better sentence."

Never:

"This game suddenly became unfair."

---

## Boss Philosophy

Biome bosses examine the player's current fluency.

The final encounter examines the player's mastery.

Bosses should not merely require faster execution.

They should require better expression.

---

## Guiding Principle

Every Technique should feel satisfying alone.

Every encounter should reward speaking The Language intentionally.
