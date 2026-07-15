# Enemy Bible

Status: Approved

## Purpose

This document defines the ordinary enemies used during the short connective stages of Irys+Ion.

Ordinary enemies exist to:

- provide meaningful danger between bosses;
- teach and rehearse mechanics used by upcoming bosses;
- reinforce the planetary AI’s visible defensive iteration;
- test adaptation to the current Irys body;
- create brief, deliberate combat encounters.

Ordinary enemies are not disposable fodder.

They should be capable of ending a run when handled poorly.

They remain substantially below boss tier in durability, complexity, and encounter length.

## Enemy Principles

Ordinary enemies are authored mechanical problems.

Ordinary enemies use authored state machines.

Their attacks must remain:

- readable;
- directional;
- avoidable;
- interruptible according to explicit state;
- consistent across runs.

Enemy difficulty should come primarily from:

- positioning;
- timing;
- attack geometry;
- movement behavior;
- interaction with nearby enemies;
- persistent run attrition.

Enemy difficulty should not come primarily from:

- inflated health pools;
- hidden stagger meters;
- procedural affixes;
- randomized move sets;
- enemy rarity tiers;
- large waves;
- dynamic scaling to the selected Irys;
- unavoidable damage.

Ordinary passive body contact is harmless.

Enemy bodies use firm ordinary-movement blocking.

Each enemy attack activation may hit Irys once unless explicitly authored as multi-hit.

## Enemy Density

Connective stages use low enemy density.

Typical encounters contain one to three enemies.

A single enemy should be capable of demanding attention.

Two enemies should create a meaningful positional problem.

Three enemies should represent a serious encounter rather than routine filler.

Large repeated waves of ordinary enemies are outside the intended structure.

Enemy placement should be deliberate and fixed.

## Threat and Durability

Ordinary enemies are significantly less durable than bosses.

They are not universally killed by:

- one Normal Technique contact;
- one Primed Technique;
- one successful interruption.

Most ordinary enemies should survive multiple Normal contacts.

Primed Techniques should be highly effective without automatically deleting every enemy.

Enemy durability should provide enough time for the player to read and engage with the enemy’s behavior.

It should not turn ordinary enemies into miniature bosses.

Enemy attacks use fixed authored damage values.

Damage does not scale as a percentage of the current Irys’s maximum structural integrity.

Enemy health and damage do not scale according to:

- the current Irys body;
- run count;
- player performance;
- previous failures;
- current remaining integrity.

A weak or damaged Irys may die quickly.

A durable Irys may survive additional mistakes.

Body-generation advantages must remain meaningful.

## Enemy Reactions

Successful player contact must always produce readable enemy-side feedback.

Responses may include:

- displacement;
- recoil;
- sparks;
- temporary hit treatment;
- sound;
- damaged animation;
- local structural deformation.

Small enemies are generally displaced more strongly.

Larger enemies may recoil with reduced displacement.

Later enemy generations may resist displacement during specific actions.

A stationary or anchored enemy must still communicate successful contact clearly.

## Enemy Interruption

Interruption is contextual.

Early enemies should be interrupted by most meaningful player contact.

Later enemies may possess:

- attacks with stronger interruption resistance;
- brief uninterruptible committed actions;
- exposed interruptible preparations;
- specific Primed punish opportunities.

Enemies do not use a universal hidden stagger meter.

Normal Techniques do not require individual stagger ratings.

Primed Techniques may be more effective at interrupting resistant actions.

The enemy’s current authored state determines whether interruption succeeds.

## The Three Enemy Lineages

The ordinary-enemy roster is organized around three conceptual lineages:

1. Striker
2. Pursuer
3. Anchor

Each lineage appears in increasingly refined forms across the three outward stages.

Later generations should visibly and mechanically descend from earlier ones.

The planetary AI improves the same defensive ideas rather than replacing its entire roster with unrelated creatures.

The three lineages should share reusable implementation components where practical.

They should still remain visually and behaviorally distinct.

## Striker Lineage

### Purpose

The Striker teaches the player to read and answer a direct attack along a clear axis.

It is an active close-range threat.

Its attacks should create obvious opportunities to:

- avoid through movement;
- pass through with a Technique;
- interrupt a committed action;
- punish recovery.

### Stage One Striker

The Stage One Striker is crude and strongly mechanical.

Possible forms include:

- a piston-driven attacker;
- a heavy industrial armature;
- a compact charging machine;
- a blunt articulated mechanism.

Its behavior should use a small attack set.

Recommended attack families:

- horizontal charge;
- overhead strike;
- floor-level sweep;
- short committed lunge.

Attacks are individually telegraphed.

Recovery is readable.

Most attacks are interruptible.

At least one stronger committed action may resist Normal interruption while remaining vulnerable to a well-timed Primed Technique.

### Stage Two Striker

The Stage Two Striker is more articulated and mobile.

It retains the Stage One lineage’s direct aggression while gaining a second attack axis.

Possible improvements include:

- horizontal and vertical attack options;
- a short reposition before striking;
- a two-part authored sequence;
- improved recovery;
- stronger interruption resistance during one committed action.

It should force the player to identify direction rather than answering every attack with the same Technique.

### Stage Three Striker

The Stage Three Striker is increasingly humanoid and Irys-like.

It does not use the complete Language.

Its movement may nevertheless suggest directional combat intention.

Possible improvements include:

- chaining two readable attacks;
- shifting between horizontal and vertical threats;
- limited mid-sequence repositioning;
- a visibly committed high-threat attack;
- shorter punish windows;
- stronger resistance during selected actions.

It should foreshadow Anti-Irys without functioning as a miniature mirror duel.

## Pursuer Lineage

### Purpose

The Pursuer controls spacing.

It prevents the player from treating the arena as static and tests course correction under pressure.

Its attacks should remain authored rather than procedurally predictive.

### Stage One Pursuer

The Stage One Pursuer advances slowly or tracks the player through simple movement.

It then commits to a readable attack.

Possible behaviors include:

- slow horizontal pursuit;
- a delayed rush;
- a short leap or drop;
- a committed attack after reaching preferred range.

Its tracking should stop when the attack is committed.

The player must be able to read the final attack geometry.

### Stage Two Pursuer

The Stage Two Pursuer is faster and more capable of intercepting movement.

Possible improvements include:

- targeting the player’s projected position;
- approaching from walls, rails, or elevated surfaces;
- changing elevation before committing;
- combining pursuit with a delayed strike;
- retreating before rapidly re-entering.

It should reward immediate Normal Technique redirection.

It must not read raw player input or react instantaneously after commitment.

### Stage Three Pursuer

The Stage Three Pursuer is articulated and increasingly Irys-like.

Possible improvements include:

- changing axis once during an authored pursuit sequence;
- using a horizontal approach followed by a vertical intercept;
- applying pressure from multiple arena surfaces;
- using a short committed sequence with a clear endpoint;
- resisting interruption during selected high-force movement.

Its behavior should foreshadow the mobility of later bosses while remaining substantially simpler.

## Anchor Lineage

### Purpose

The Anchor controls part of the arena.

It creates movement lanes, unsafe regions, or timing problems without directly pursuing the player.

The Anchor teaches the player to use The Language around persistent spatial threats.

### Stage One Anchor

The Stage One Anchor is fixed or heavily constrained.

Possible forms include:

- turret;
- piston;
- rotating sweep mechanism;
- ceiling press;
- floor-mounted emitter.

Recommended attack families:

- repeated horizontal lane attack;
- vertical strike zone;
- sweeping arm;
- clearly telegraphed projectile;
- temporary environmental hazard.

The Anchor must remain directly attackable.

It should not force the player to wait through extended protected states.

### Stage Two Anchor

The Stage Two Anchor can reposition or articulate its threat zone.

Possible improvements include:

- rotating between surfaces;
- relocating along a fixed track;
- changing the orientation of its attack lane;
- combining two previously separate threat zones;
- briefly retracting before reappearing elsewhere.

Its movement remains predictable and authored.

It should create positional problems when paired with a Striker or Pursuer.

### Stage Three Anchor

The Stage Three Anchor resembles a restrained limb, partial body, or biomechanical structure.

It foreshadows the Restrained Proto-Irys boss.

Possible improvements include:

- large articulated sweeps;
- grasping or pressing actions;
- multiple connected threat points;
- exposed joints after committed attacks;
- selected uninterruptible emergency actions;
- high-value Primed interruption opportunities.

It remains an ordinary enemy and should not approach boss-level encounter length or health.

## Stage Distribution

### Stage One

Stage One introduces the crude generation of each lineage.

Encounters should emphasize individual enemies or simple pairs.

The player should learn:

- direct attack reading;
- enemy displacement;
- basic interruption;
- ordinary-enemy durability;
- the danger of persistent damage.

Recommended encounter compositions include:

- one Striker;
- one Pursuer;
- one Anchor;
- Striker plus Anchor;
- Pursuer plus Anchor.

### Stage Two

Stage Two introduces the articulated generation of each lineage.

Encounters may more frequently combine two complementary enemies.

The player should practice:

- immediate redirection;
- changing axis;
- movement under pursuit;
- attacking through controlled space;
- identifying committed actions.

Recommended compositions include:

- Stage Two Striker plus Anchor;
- Pursuer plus Striker;
- Stage Two Pursuer plus earlier-generation Anchor;
- one advanced lineage enemy supported by one earlier-generation enemy.

### Stage Three

Stage Three introduces increasingly Irys-like lineage forms.

Encounters should remain sparse.

The player should practice:

- contextual interruption;
- stronger committed enemy attacks;
- limb-like threat geometry;
- vertical and horizontal pressure;
- Prime use under danger;
- preparation for the Restrained Proto-Irys.

Recommended compositions include:

- one Stage Three lineage enemy;
- one Stage Three enemy paired with one earlier-generation enemy;
- two complementary Stage Three enemies in a major pre-boss encounter.

Three Stage Three enemies should be used only when the arena and telegraphs remain clearly readable.

## Reuse Across Stages

Earlier generations may reappear during later stages.

Reuse should create deliberate mechanical combinations rather than simply increase population.

Examples include:

- an early Anchor controlling space while a later Pursuer applies pressure;
- a crude Striker supporting a Stage Two Anchor;
- an earlier Pursuer creating predictable movement pressure during a Stage Three encounter.

Earlier enemies should not receive arbitrary health inflation to remain relevant.

Their behavior and placement should provide continued value.

## Return Route

The return route may reuse established enemies in altered placements.

It should not introduce a fourth generation or an additional enemy lineage.

Return-route encounters should be brief and urgent.

Enemy reuse may show:

- damaged systems;
- emergency deployment;
- altered arena access;
- unusual combinations;
- loss of planetary-AI control.

The return route must not become another full combat stage.

## Relationship to Bosses

Ordinary enemies prepare the player for the fixed boss sequence.

### Preparation for the First Guardian

Stage One enemies teach:

- clear directional attacks;
- large telegraphs;
- interruption;
- direct movement through threats.

### Preparation for the Second Guardian

Stage Two enemies teach:

- pursuit;
- redirection;
- multiple attack axes;
- changing direction under pressure.

### Preparation for the Restrained Proto-Irys

Stage Three enemies teach:

- limb-like attacks;
- contextual interruption;
- committed actions;
- biomechanical geometry;
- Prime punish opportunities.

The Control-Center Mech and Anti-Irys recombine lessons already established by enemies and previous bosses.

Ordinary enemies do not need to reproduce those final encounters directly.

## Drops and Rewards

Ordinary enemies do not require item drops.

They do not provide:

- currency;
- equipment;
- cards;
- permanent upgrades;
- consumable healing;
- randomized loot.

Their reward is successful traversal and preserved structural integrity.

Any environmental or narrative object associated with an enemy should be authored for the stage rather than generated as loot.

## Generated Irys Compatibility

Every ordinary enemy must remain beatable by every viable generated Irys.

Enemy placement and attack geometry must not require:

- rare jump power;
- exceptional Advance distance;
- a particular limb configuration;
- unusually high structural integrity;
- a specific Prime hold duration;
- a specific displacement-resistance value.

Strong body rolls should remain advantageous.

Weak bodies may require cleaner execution.

Enemy behavior must not dynamically scale to remove those differences.

## Implementation Model

The implementation should favor reusable components across each lineage.

Attack selection may use contextual weighted randomness.

Valid attack pools may depend on:

- position;
- elevation;
- cooldown;
- encounter state;
- recent attacks.

Anti-repeat weighting should prevent monotonous repetition.

Once selected, attacks retain fixed:

- telegraph;
- timing;
- movement;
- hitboxes;
- damage;
- recovery.

Do not use untelegraphed random timing mutation.

Useful shared components may include:

- authored state machine;
- telegraph state;
- committed attack state;
- recovery state;
- contextual interruption flag;
- damage reaction;
- displacement reaction;
- target-position sampling;
- fixed movement path;
- attack hitbox activation;
- per-activation hit registration.

Later generations should extend or recombine lineage behavior rather than require entirely unrelated architectures.

The system does not require procedural enemy assembly.

## Implementation Requirements

The implemented ordinary-enemy roster must include:

- three enemy lineages;
- three visible generations of each lineage;
- low-density authored encounters;
- fixed placements and behaviors;
- meaningful fixed damage;
- durability substantially below bosses;
- survival through multiple Normal contacts for most enemies;
- strong but not universally lethal Primed contact;
- readable enemy-side hit feedback;
- contextual interruption;
- increasingly resistant later actions;
- visible progression toward Irys-like forms;
- reuse of earlier generations in later combinations;
- no routine wave arenas;
- no dynamic scaling;
- no procedural affixes;
- no randomized move sets;
- no loot or reward drops;
- compatibility with all viable Irys bodies.

## Shutdown Return Behavior

After the planetary AI transfers into Anti-Irys and leaves the control center, the return route reuses existing enemy generations without introducing a fourth generation, elite class, modifier system, or new lineage.

These enemies no longer operate under the planetary AI’s precise central coordination. Their behavior should feel degraded, unstable, and more dangerous while remaining authored and learnable.

Return-route encounters may vary:

- initial enemy state;
- activation order;
- idle timing;
- encounter pairing;
- starting position;
- environmental context;
- limited attack-selection weighting.

Shutdown behavior must not rely on:

- untelegraphed attacks;
- arbitrary universal speed multipliers;
- random cancellation of authored attacks;
- unreadable timing changes;
- procedural behavior mutation;
- violations of established interruption rules.

The planetary-AI lighting is absent from these enemies and the surrounding facility. Their increased danger comes from lost coordination, unstable operating conditions, and deliberate encounter composition rather than hidden statistical scaling.

## Scope Boundaries

The Enemy Bible does not require:

- additional enemy lineages;
- dozens of unrelated enemy species;
- procedural enemy generation;
- randomized traits;
- elite rarity tiers;
- loot tables;
- wave-survival rooms;
- dynamic difficulty scaling;
- generalized squad AI;
- large crowds;
- boss-level health;
- complete final animation frame data before implementation.

Additional enemies should be added only when the existing lineages cannot teach or support a required boss mechanic.
