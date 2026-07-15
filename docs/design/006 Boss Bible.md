# Boss Bible

Status: Approved

## Purpose

This document defines the fixed five-boss sequence that forms the primary combat content of Irys+Ion.

The game is boss-focused.

Boss encounters should account for most active combat time.

Connective stages exist primarily to introduce mechanics, establish environments, deliver sparse narrative, and provide pacing between major fights.

The Boss Bible defines encounter purpose, governing rules, phase structure, and the minimum authored behaviors required for implementation.

It does not require final numerical tuning, complete animation lists, or exhaustive attack-frame data before implementation begins.

## Boss Principles

Every boss is a readable mechanical problem built around The Language.

Bosses should test:

- positioning;
- directional intention;
- Normal Technique redirection;
- Primed Technique commitment;
- target selection;
- interruption judgment;
- adaptation to the current Irys body;
- recognition of authored attack states.

Bosses should not rely primarily on:

- large waves of ordinary enemies;
- hidden stagger meters;
- long invulnerability sequences;
- unavoidable damage;
- percentage-based damage;
- health scaling against the current Irys;
- procedural attack generation;
- rotating boss rosters;
- randomized phase order;
- mandatory body configurations.

Every viable generated Irys must be capable of defeating every boss.

A stronger Irys may complete a fight faster or survive more mistakes.

A weaker Irys may require cleaner execution.

Boss scaling must not erase the meaning of body generation.

## Fixed Boss Sequence

Every run uses the same fixed sequence:

1. First Guardian
2. Second Guardian
3. Restrained Proto-Irys
4. Control-Center Mech
5. Anti-Irys

The sequence does not rotate.

The route does not branch.

Bosses cannot be permanently skipped.

Player progression comes from learning these encounters and adapting that knowledge to different Irys bodies.

## Boss Health

Each boss has fixed authored maximum health.

Boss health does not scale according to:

- the current Irys’s maximum integrity;
- Strike Force;
- selected traits;
- run count;
- prior failures;
- player performance.

Powerful bodies should genuinely defeat bosses faster.

Weak bodies should genuinely require more successful contact.

Each boss uses one persistent health bar across all phases.

Phase transitions occur at authored health thresholds.

The health bar does not refill between phases.

Damage dealt before a transition remains meaningful.

## Boss Damage

Boss attacks deal fixed authored damage values.

Damage does not scale as a percentage of the current Irys’s maximum integrity.

Different attacks may deal different amounts according to:

- telegraph length;
- contact difficulty;
- area coverage;
- interruption resistance;
- positional consequence;
- authored threat.

A durable Irys may survive more mistakes.

A fragile Irys may be defeated by attacks that stronger bodies can endure.

No ordinary boss attack should automatically scale into the same number of required hits against every body.

## Vulnerability and Phase Transitions

Bosses remain vulnerable through most of each encounter.

Damage access should come from reading movement and finding safe contact rather than waiting through long invulnerable sequences.

A boss may become briefly invulnerable during a clearly signaled phase transition.

Transition immunity must remain short.

A transition may include:

- a pose change;
- mechanical reconfiguration;
- arena adjustment;
- dialogue;
- a brief visual escalation;
- a new stance.

It must not become a prolonged spectacle sequence that substantially interrupts combat.

Later phases should intensify, recombine, or extend the boss’s established combat problem.

They should not replace the encounter with an unrelated mechanic.

## Interruption

Boss interruption is contextual.

Bosses do not use a universal hidden stagger meter.

Specific attacks, limbs, states, or vulnerability windows may be interruptible.

Other attacks may be explicitly resistant or uninterruptible.

Interruptibility should be communicated through:

- telegraph language;
- animation;
- posture;
- exposed structure;
- repeated player observation;
- clear reaction when interruption succeeds or fails.

Normal Techniques do not require individual stagger ratings.

Primed Techniques may possess stronger interruption capability according to the Player Feel Specification.

The boss’s current state remains authoritative.

## Contact and Reaction

Successful player contact must always produce readable boss-side feedback.

Depending on the boss and contact location, this may include:

- recoil;
- limb displacement;
- local hit treatment;
- sparks;
- exposed damage;
- temporary color change;
- sound;
- arena response;
- a brief authored Primed impact hold.

Large bosses may remain physically stationary while still providing unmistakable hit confirmation.

Normal Technique contact does not pause, slow, or interrupt Irys.

Primed Technique contact may use short, fixed, learnable impact responses.

## Adds

Routine add waves are not part of the primary boss structure.

A boss may use a small number of secondary objects or hazards when they are integral to its central mechanic.

These may include:

- detachable components;
- environmental mechanisms;
- limited projectiles;
- temporary constructs;
- destructible arena elements.

Do not use repeated ordinary-enemy waves to lengthen encounters or create artificial difficulty.

## Boss Phase Count

The initial implementation target is:

- First Guardian: two phases
- Second Guardian: two phases
- Restrained Proto-Irys: two phases
- Control-Center Mech: three phases
- Anti-Irys: three phases

These counts may be adjusted through playtesting if an encounter becomes too short, repetitive, or overextended.

Phase count is not a content quota.

Each phase must materially evolve the existing fight.

## First Guardian

### Role

The First Guardian is the player’s first major mechanical examination.

It is crude, industrial, and strongly non-Irys-like.

It should feel like an early defensive solution built around force, machinery, and obvious attack geometry.

### Primary Test

The encounter tests:

- individual use of Advance, Ascend, and Descend;
- movement through readable attacks;
- basic enemy traversal;
- recognition of large telegraphs;
- first use of Prime against an exposed opportunity;
- recovery from mistakes.

### Encounter Shape

The guardian occupies a compact industrial arena.

It should use large, readable movements rather than high speed.

Possible attack families include:

- a horizontal ram or sweeping arm;
- an overhead press;
- a floor-level sweep;
- a vertical piston or falling strike;
- a clearly telegraphed committed charge.

The player should be able to answer its attacks through The Language rather than waiting passively at arena edges.

### Phase One

Phase One introduces the boss’s principal attack families individually.

Telegraphs should be long and visually distinct.

The boss remains vulnerable through most actions.

At least one committed attack should expose an interruption or Primed punish opportunity.

### Phase Two

Phase Two combines previously introduced attacks and increases arena pressure.

The boss may:

- shorten gaps between actions;
- chain two familiar attacks;
- damage part of the arena;
- expose a faster recovery;
- use a larger version of its committed charge.

The phase should test recognition and execution, not introduce an unrelated system.

## Second Guardian

### Role

The Second Guardian is more articulated, mobile, and adaptive.

It represents the planetary AI moving away from crude machinery toward a combat form capable of reacting to Ion.

It may hint at humanoid geometry without closely resembling Irys.

### Primary Test

The encounter tests:

- immediate Normal Technique redirection;
- chaining;
- changing direction under pressure;
- abandoning an unfinished Technique;
- attacking from multiple axes;
- adapting Technique choice to moving targets.

### Encounter Shape

The boss should move through the arena rather than remaining fixed.

It may use walls, ceilings, rails, articulated limbs, or rapid changes in elevation.

Possible attack families include:

- a tracking horizontal pursuit;
- a vertical intercept;
- a delayed strike aimed at the player’s projected path;
- a sweeping articulated limb;
- a retreat followed by a rapid re-entry;
- a committed attack with a readable interruption opportunity.

The encounter should punish repeating one direction without thought.

### Phase One

Phase One establishes movement and directional pursuit.

The player learns that the boss responds to current position but remains governed by readable authored attacks.

### Phase Two

Phase Two chains established movements more aggressively.

The boss may:

- redirect once during a sequence;
- attack from a new arena surface;
- shorten repositioning time;
- combine horizontal and vertical threats;
- expose shorter but still readable Primed punish windows.

The fight remains authored and fixed across runs.

It does not procedurally adapt to the player’s inputs.

## Restrained Proto-Irys

### Role

The third guardian is a large, recognizably Irys-like experimental body attached to, restrained against, or integrated into the environment.

It is the planetary AI’s incomplete attempt to reproduce the Irys form.

The fight should make that resemblance unmistakable without presenting a normal humanoid duel.

### Primary Test

The encounter tests:

- target selection;
- vertical positioning;
- limb-specific threats;
- contextual interruption;
- recognition of interruptible and uninterruptible attacks;
- Primed Technique commitment;
- movement through a large enemy body;
- attacking exposed structures.

### Arena

The boss is integrated into a large biomechanical chamber.

Its body remains anchored or restrained.

The player fights its arms, legs, restraints, exposed joints, or supporting machinery.

Damageable limbs and components feed the boss’s one persistent health bar.

Destroying or disabling a component may alter authored behavior without creating a separate health encounter.

### Phase One

Phase One centers on the upper body or arms.

Possible attack families include:

- arm sweeps across large arena regions;
- targeted grabs;
- descending limb strikes;
- exposed joints after committed attacks;
- interruptible attempts to break restraints;
- uninterruptible emergency movements.

The player learns which actions can be interrupted and which must be avoided.

### Phase Two

Phase Two activates additional limbs, lower-body structures, or a partially freed torso.

The boss may:

- attack from more than one axis;
- alter safe regions;
- expose its central structure;
- use stronger uninterruptible actions;
- create high-value Primed punish opportunities.

The phase should feel like the experiment becoming more complete, not like a separate creature replacing it.

### Post-Boss Function

The nearby substrate room uses pod-like systems resembling the Ark.

This confirms that the planetary AI is approaching Irys’s production method.

## Control-Center Mech

### Role

The Control-Center Mech is the apparent final boss.

It represents the planetary AI’s technical apex before the mirrored Ark reveal.

It should be fast, formidable, and mechanically sophisticated while remaining visually distinct from Irys.

### Primary Test

The encounter combines the lessons of the first three bosses:

- directional reading;
- immediate redirection;
- vertical and horizontal chaining;
- contextual interruption;
- Primed commitment;
- arena awareness;
- recovery discipline;
- adaptation to the current body.

### Encounter Shape

The mech should move confidently through the arena and exert pressure without relying on adds.

Possible attack families include:

- high-speed horizontal traversal;
- aerial intercepts;
- targeted descending attacks;
- articulated area control;
- beams or projectiles that define temporary movement lanes;
- a heavily committed attack with a major interruption opportunity;
- short defensive reconfiguration.

The mech remains attackable through most of the encounter.

### Phase One

Phase One establishes the mech’s mobility and primary attack vocabulary.

It should feel more technically complete than prior guardians.

### Phase Two

Phase Two combines movement and arena control.

The mech may:

- chain horizontal and vertical attacks;
- use projectiles to shape Technique decisions;
- shorten recovery after ordinary attacks;
- expose only selected interruption windows;
- demand more precise use of Prime.

### Phase Three

Phase Three presents the mech’s highest-pressure form.

It should recombine all established attack families into dense but readable sequences.

The final phase may include:

- faster transitions;
- a damaged visual state;
- more aggressive pursuit;
- a final committed attack;
- a short, clear phase transition.

The mech should feel like a plausible final boss.

Its defeat triggers the planetary AI’s Ark reveal.

## Anti-Irys

### Role

Anti-Irys is the final boss.

The opposing Irys carries the planetary AI in blade-form.

The encounter is the culmination of the planetary AI’s observation of Ion’s repeated infiltration success.

It has concluded that the Irys-and-blade configuration is the superior combat model.

### The Same Language

Anti-Irys uses the same combat Language as the player:

- Advance;
- Ascend;
- Descend;
- Prime.

Her exact distances, timing values, scale, and authored combinations may be boss-specific.

Her intention must remain readable through the same directional vocabulary the player has mastered.

She must not use unrelated conventional boss attacks disguised as The Language.

### Shared Behavioral Logic

Anti-Irys should visibly obey the fundamental distinction between Normal and Primed Techniques.

Her Normal Techniques are:

- responsive;
- directional;
- chainable;
- redirectable;
- capable of ending before contact.

Her Primed Techniques are:

- visibly armed;
- committed;
- uninterruptible by ordinary contact;
- weighty;
- punishable after authored recovery.

Her Prime state must use a readable world-space cue analogous to the player’s armed Prime cue.

### Primary Test

The encounter tests complete fluency:

- reading another speaker of The Language;
- identifying current intention;
- reacting to immediate redirection;
- recognizing Prime;
- punishing commitment;
- choosing when to remain fluid;
- choosing when to commit;
- controlling spacing against a similarly mobile opponent.

### Arena

The fight occurs in the compromised Ark.

The arena remains visibly connected to the candidate chamber and dormant pods.

Ordinary movement collision between the two bodies may apply normally.

While either combatant is using a Technique, enemy-body collision does not stop that Technique.

Solid terrain remains authoritative.

### Phase One

Phase One establishes the mirror.

Anti-Irys primarily uses Normal Advance, Ascend, and Descend.

She chains them in readable sequences and demonstrates that she can redirect before contact.

The player should recognize familiar intention even when the exact boss timing differs.

### Phase Two

Phase Two introduces regular Prime use.

Anti-Irys visibly arms Prime, repositions, and commits to Primed Techniques.

Her Primed attacks should be dangerous but strongly telegraphed.

Successful avoidance should create meaningful punish opportunities during authored recovery.

### Phase Three

Phase Three represents the planetary AI’s fullest fluency.

Anti-Irys combines:

- rapid Normal redirection;
- repeated changes of axis;
- deliberate Prime use;
- spacing traps;
- attacks that exploit the Ark arena;
- shorter decision windows without abandoning readability.

The phase should not grant her arbitrary abilities outside The Language.

Difficulty comes from fluent sequencing, pressure, and commitment judgment.

### Fairness

Anti-Irys must remain readable.

She may be faster or more technically exact than the player, but she must not:

- ignore her own Prime commitment;
- cancel Primed recovery;
- attack without directional intention;
- use invisible input;
- gain universal invulnerability;
- deal percentage-based damage;
- become immune for extended periods.

The final duel should feel like mastery tested against another speaker, not a rules exception.

## Generated Irys Compatibility

Boss encounters must accommodate substantial variation in the player’s body.

No boss may require:

- a minimum jump height available only to rare bodies;
- a specific limb configuration;
- a specific Advance distance;
- a specific Prime hold duration;
- a specific damage threshold;
- a specific displacement resistance value;
- an exceptional aesthetic or rarity state.

Arena geometry and attack timing should be validated against low-performing but viable bodies.

Strong body rolls should remain advantageous.

The game should not normalize them away.

## Persistent Run Attrition

Damage persists across the complete run according to the Progression Specification.

Bosses must therefore be difficult without assuming that the player begins every fight at full integrity.

Post-boss substrate transfer provides the primary recovery and build-change decision.

The boss sequence should not compensate by dynamically reducing damage or health according to current integrity.

## Implementation Priorities

The initial implementation must prioritize:

- readable telegraphs;
- stable authored attack sequences;
- fixed health and damage values;
- one health bar per boss;
- short phase transitions;
- high boss vulnerability uptime;
- contextual interruption;
- compatibility with varied Irys bodies;
- minimal add use;
- direct engagement;
- reliable hit feedback;
- clear Primed punish windows;
- a final boss that visibly uses The Language.

Exact health values, damage values, timings, phase thresholds, acceleration curves, and recovery windows should remain data-driven for playtesting.

## Control-Center False Finale and Transfer

The fourth boss is a tall knight-like bipedal command mech.

It must be:

- unmistakably mechanical;
- refined and immaculate;
- broadly neutral or masculine in proportion;
- visually distinct from Irys and Anti-Irys;
- capable of expressing the complete directional combat vocabulary;
- equipped with integrated or projected cutting structures rather than a true counterpart to Ion.

Its defeat must plausibly appear to be the conclusion of the campaign.

After the command mech falls, the player enters an adjoining containment chamber without another enemy encounter.

Anti-Irys is present in this chamber as a fixed, completed, inactive substrate. She is suspended by tendrils, contained within a partial vertical pod, or supported above a lift platform.

The planetary AI’s ultraviolet, chartreuse, and sterile-white energy then drains from:

- room panels;
- conduits;
- distant machinery;
- control surfaces;
- structural lighting.

That energy converges through the support tendrils and activates Anti-Irys.

The planetary AI manifests as the counterfeit blade in Anti-Irys’s hand. The blade appears during this transfer rather than existing as an ordinary stored weapon.

The planetary AI explains that Ion’s penetration of the control center has freed it from its previous confinement. The Anti-Irys substrate provides it with a mobile wielder, while the planetary AI itself persists as the blade.

It also reveals that Ion’s repeated returns have exposed the Ark’s location.

Anti-Irys does not fight the player in the control center. She departs toward the Ark by a route unavailable to the player, forcing Irys and Ion to return through the shutting-down facility.

## Anti-Irys

Anti-Irys uses one fixed authored body design. She is not generated from, copied from, or mechanically derived from the player’s current Irys substrate.

Mechanically, Anti-Irys is the planetary AI’s complete answer to the Irys–Ion configuration:

- Anti-Irys is the wielder;
- the planetary AI is the blade;
- Normal Techniques remain responsive and redirectable;
- Primed Techniques remain committed through their authored completion and recovery;
- the encounter must obey the same fundamental Language rules imposed on the player;
- the boss must not cheat through hidden action cancellation, arbitrary tracking, or exceptions that invalidate the player’s learned combat grammar.

Anti-Irys may possess boss-authored values, attacks, combinations, and phase behavior, but the duel must remain readable as two opposed users of the same Language.

## Final Ark Duel

The final encounter occurs in the Ark’s main pod chamber among the dormant Irys candidates.

The Ark intelligence actively attempts to protect the pods, but this protection is expressed through scripted arena choreography rather than a secondary defense objective.

Across authored phase transitions, the Ark may:

- raise transparent barriers;
- deploy armored shutters;
- retract pod banks into the walls or floor;
- move pods along overhead tracks;
- lock damaged partitions into place;
- concentrate its remaining protection around a smaller surviving cluster.

These interventions may reshape movement lanes, create terrain impacts, block long committed paths, and alter vertical routes.

The player is never responsible for protecting individual pods.

There must be:

- no pod health bars;
- no pod-defense failure state;
- no performance-dependent ending variation;
- no escort mechanics;
- no requirement to interrupt specific attacks solely to preserve pods.

Any visible pod damage or loss is scripted at fixed encounter transitions.

The Ark does not directly attack Anti-Irys. Its role is to preserve the dormant candidates, reshape the arena, and eventually prevent the planetary AI’s escape.

The final fight remains a three-phase mastery test centered on the player’s use of The Language.

## Planetary AI Defeat

When Anti-Irys is defeated, the planetary-AI blade rises from the fallen substrate in a direct visual parallel to Ion rising from a defeated Irys body.

The blade attempts to phase, teleport, or return to its former control-center chamber.

Irys—the Ark intelligence—prevents the escape by containing, seizing, or locking the blade within the Ark chamber.

The planetary AI is therefore exposed without a functioning wielder and without access to the planetary infrastructure it previously controlled.

Ion delivers the direct killing blow to the planetary-AI blade.

The victory belongs jointly to both persistent protagonists:

- Irys prevents escape;
- Ion destroys the planetary AI.

## Scope Boundaries

The Boss Bible does not require:

- procedural boss adaptation;
- randomized attacks without authored states;
- alternate boss rosters;
- optional superbosses;
- branching boss routes;
- permanent boss skips;
- large add waves;
- multiple independent health bars per phase;
- percentage-based damage;
- dynamic boss scaling;
- generalized limb simulation;
- complete animation frame data before implementation.

Additional mechanics should be added only when necessary to make one of the five authored encounters function.
