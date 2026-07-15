# Player Feel Specification

Status: Approved

## Purpose

This document defines how Irys responds to player input during Techniques.

The governing requirement is:

> Player intention is always respected.

Commitment is also an expression of intention. Responsiveness does not mean that every action can be escaped without consequence.

## Technique Authority

A Technique has authority over Irys’s movement while it is active.

Normal and Primed Techniques handle that authority differently.

Ordinary movement applies only while no Technique or authored reaction has authority.

When a Technique input and ordinary movement would otherwise take effect at the same time, the Technique takes precedence.

When ordinary movement has authority, A and D change Irys’s facing immediately, including while airborne.

Advance captures facing at activation.

Ordinary movement input during a Technique does not steer, bend, reverse, or redirect that Technique.

Ordinary jumping may use short tunable coyote time, ordinary-jump buffering, and variable jump height.

Ordinary-jump buffering is not Technique buffering.

The base movement model does not include double jump, wall jump, dash, or teleport.

## Input Retention

There is no general Technique input queue or buffer.

Technique inputs resolve when they are entered and control is available. Inputs entered while control is unavailable are not retained for later execution.

Prime is the sole retained combat intention. It is an explicit armed state rather than buffered Technique input.

## Normal Techniques

Normal Techniques are fluid, responsive, and highly chainable.

A new Normal Technique may immediately take authority from an active Normal Technique.

The new Technique begins from Irys’s current position. Displacement already produced by the outgoing Technique remains, but its unfinished movement does not constrain the incoming Technique.

The incoming Technique imposes its own movement behavior rather than inheriting or combining with the outgoing Technique’s remaining movement vector.

For example, activating Advance during Ascend does not require Irys to continue moving upward into a diagonal trajectory. Advance immediately replaces Ascend’s movement from the position already reached.

Mechanical authority transfers immediately when the new Technique is activated.

Animation may smooth or visually reinterpret the transition, but it must not delay the incoming Technique’s movement, attack timing, hit geometry, or control.

A Normal Technique may be redirected before it contacts a target. If authority transfers before contact occurs, the outgoing Technique ends without producing a hit.

Normal Techniques remain immediately interruptible and replaceable throughout every phase of their execution, including acceleration, travel, contact-adjusted clearance, deceleration, attack presentation, and visual recovery.

Normal Technique transitions therefore create a continuous player-authored trajectory without requiring each Technique to complete or contact a target before the next begins.

## Repeated Normal Techniques

Activating the same Normal Technique while it is already active counts as a fresh activation.

The repeated Technique immediately takes authority from its previous activation and begins again from Irys’s current position.

Its movement and attack behavior restart as they would for any other new Normal Technique activation.

Repeated use does not require an intervening Technique and is not restricted by a same-Technique lockout.

A repeated activation does not automatically produce another hit against an enemy Irys is already overlapping. Repeated damage requires full separation and renewed contact.

## Normal Technique Completion and Recovery

A Normal Technique imposes no mandatory mechanical recovery.

After contacting a target, it remains immediately redirectable by another Normal Technique.

If no subsequent Technique is activated, Irys may display an authored visual recovery. This recovery is presentation only and may be interrupted immediately by new player input.

When a Normal Technique reaches its natural mechanical endpoint, its authority ends immediately.

Ordinary movement, gravity, and other normal environmental forces resume only if another Technique does not immediately take authority.

A chained Technique transfers control directly from the outgoing Technique. There is no intervening locomotion, landing pause, or neutral state.

For example, Irys may Advance through an enemy and immediately Advance again from the position reached on the enemy’s opposite side. Ordinary movement does not briefly occur between the two activations.

If no subsequent Technique is activated, ordinary movement and environmental physics resume immediately even if a visual recovery animation continues.

For example, if a Normal Technique ends while Irys is airborne, she begins falling under gravity rather than remaining suspended until the recovery animation finishes.

## Normal Technique Timing and Motion Profiles

Each Normal Technique has a fixed authored duration.

Its duration does not change according to Irys’s starting velocity, grounded state, airborne state, enemy contact, enemy size, or contact-adjusted travel distance.

Each Normal Technique may define its own normalized motion profile across that duration.

Advance reaches near-full speed immediately, with minimal acceleration, then carries that motion through its fixed duration.

Ascend launches immediately, rises quickly, and decelerates toward its endpoint.

Descend begins with an immediate downward directional response and accelerates through the remainder of its fixed duration.

These profiles are initial implementation targets rather than immutable tuning values. Their exact curves may be adjusted through playtesting without changing the governing control rules.

Every Normal Technique remains immediately interruptible throughout its full motion profile.

## Technique Exit Velocity

Each Technique defines its own authored exit velocity.

When a Technique ends naturally, ordinary physics resumes from that exit velocity unless another Technique immediately takes authority.

Exit velocity is defined per Technique rather than inherited through a universal momentum rule.

This model is provisional and should be evaluated during playtesting. Individual Technique exits or the governing model may be revised if transitions feel inconsistent or unintuitive.

## Primed Techniques

Primed Techniques are deliberate, committed, weighty, and contextually powerful.

Once a Primed Technique begins, it retains authority until its defined completion.

A Primed Technique cannot normally be interrupted or redirected by another Technique.

Technique inputs made during a Primed Technique are not buffered. The player must issue the next Technique after the Primed Technique completes.

Ordinary movement input does not redirect a Primed Technique. Any movement that occurs during its execution is part of the Technique’s authored behavior.

A Primed Technique may include locked recovery as part of its authored execution.

The Primed Technique retains authority through that recovery. Player control, ordinary movement, gravity, and the ability to activate another Technique return only when the full Primed Technique, including its recovery, has completed.

Priming every Technique is therefore naturally clumsy when used indiscriminately. Repeated Primed Techniques repeatedly surrender responsiveness and chaining opportunities without requiring stamina, cooldowns, or arbitrary usage restrictions.

## Armed Prime State

Speaking Prime arms the next Technique.

Ordinary movement does not consume or cancel the armed Prime state. The player may reposition before activating the Primed Technique.

Prime may be armed during an active Normal Technique.

Arming Prime does not interrupt that Normal Technique.

Prime remains armed until it is consumed by the next Technique, cleared by enemy interruption, or its defined hold duration expires.

Prime provides no invulnerability.

The armed state must be communicated through a persistent world-space cue rather than depending on the HUD.

The default cue should live on Ion. It may combine a steady visual charge with a restrained ringing tone.

The cue remains at full intensity throughout the armed duration. It does not decay or imply that the Primed Technique becomes weaker over time.

When Prime expires, it falls off immediately and completely.

Expiration produces a small, distinct visual and/or audio cue so the loss of the armed state is readable. Expiration imposes no penalty, recovery, or residual effect.

The Prime hold duration is a tunable value. Changing that duration does not change the strength of the resulting Primed Technique.

## Movement During Techniques

While a Technique has authority, ordinary movement input does not steer, bend, or reshape its trajectory.

The player changes direction during a Normal Technique by activating another Normal Technique.

Primed Techniques retain their authored movement until completion.

Ordinary movement resumes only when no Technique or authored reaction has authority.

## Normal Technique Hit Response

Normal Technique damage is contact-driven.

A valid hit occurs when the active Technique first makes eligible contact with an enemy.

Player attack collision uses authored swept Ion volumes aligned with the visible sword trail.

A successful Normal Technique hit does not interrupt, pause, slow, recoil, or otherwise alter Irys’s movement, timing, or control.

Normal Technique hits do not use hit-stop.

Contact must instead be communicated through enemy response and presentation such as displacement, recoil, temporary visual treatment, particles, sound, or screen response.

The exact visual treatment is not defined here, but successful contact must always be readable.

## Enemy Displacement and Interruption

A successful hit always produces readable enemy-side feedback.

The physical response depends primarily on the enemy’s size, tier, and current action.

Small enemies are generally displaced and interrupted.

Larger enemies may recoil with reduced displacement and may resist interruption.

Very large enemies and bosses may remain physically stationary. When they do, temporary visual and audio feedback must still clearly confirm that the attack landed.

Whether an enemy’s current action is interrupted is contextual rather than universal.

Late-game enemies may possess explicitly uninterruptible actions.

Bosses may expose only particular interruptible attacks or windows. Discovering those vulnerabilities may form part of the strategy for defeating them.

Normal Techniques do not require a hidden hierarchy of individual stagger or interruption strengths. Their primary distinctions remain geometric intention and combat purpose.

Primed Techniques may possess greater interruption capability, but the enemy’s size, tier, current action, and authored resistance still govern the result.

## Primed Technique Contact Response

A Primed Technique may have authored execution changes when it successfully contacts a target.

These responses may include a brief impact hold, penetration, binding, recoil, altered follow-through, passing through the target, stopping at contact, or lodging temporarily in the target.

Primed enemy-contact behavior is defined per Technique according to its geometric intention.

For example, a Primed Descend striking a limb or small enemy may dwell briefly at the point of contact to add weight to the successful impact.

The response is authored for the Primed Technique and relevant contact category rather than calculated dynamically from damage, enemy mass, or procedural physics.

Each defined response uses fixed, learnable timing.

A successful impact may extend the Primed Technique’s total committed duration.

That extension must remain short and tightly bounded. It should add weight without making success feel punitive or making the Technique’s timing difficult to learn.

## Irys Taking Damage

A valid enemy hit interrupts and displaces Irys during a Normal Technique unless that Technique explicitly avoids, deflects, or withstands the attack.

The interrupted Normal Technique ends without completing any unfinished movement or attack.

Each enemy or boss attack activation may damage Irys once unless that attack is explicitly authored as multi-hit.

Lingering overlap from the same attack activation cannot repeatedly damage Irys.

Enemy-authored knockback is the same for all generated frames.

Hit-reaction lockout may vary by frame.

Enemy interruption clears:

- the active Normal Technique;
- the armed Prime state;
- any other transient Technique state.

Inputs entered during the resulting hit reaction are not queued, buffered, or retained.

The player must express a new intention after control returns.

Retaining Prime through damage is not part of the base behavior.

## Damage During Primed Techniques

Primed Techniques provide strong resistance to interruption.

Ordinary enemy hits may still damage Irys, but they do not cancel, redirect, recoil, or alter the authored execution of the Primed Technique.

Only exceptionally forceful enemy attacks may break a Primed Technique’s commitment.

This interruption threshold must be high, uncommon, and clearly communicated by the attacking enemy.

When an exceptionally forceful attack breaks the Primed Technique, its execution ends and Irys enters the enemy-authored hit reaction.

A non-interrupting hit during a Primed Technique must still provide clear damage feedback through presentation such as sparks, flashes, sound, or another world-space response.

Damage readability must not depend on the existence of a health bar or other HUD element.

## Enemy-Authored Hit Reactions

An interrupting enemy attack may impose a brief authored hit reaction and recovery.

The hit reaction fully controls Irys until its recovery ends.

Ordinary movement and Technique input cannot steer, cancel, or shorten the reaction.

Inputs entered during the reaction are ignored and are not retained.

Ordinary hits should return control quickly. Stronger attacks may impose longer reactions.

If the displacement carries Irys into a wall, floor, ceiling, or other solid terrain before the reaction finishes, the original displacement ends at contact.

It then transitions into a brief contextual environmental-impact reaction.

The terrain stops or redirects Irys’s movement rather than allowing the original displacement to continue against the surface.

Control returns when the applicable hit or environmental-impact recovery ends.

## Normal Technique Terrain Collision

Solid terrain immediately terminates an active Normal Technique at contact.

The collision may produce a brief visual impact response, but it adds no locked recovery.

The Technique’s remaining movement and any unfinished attack are cancelled.

Ordinary movement, gravity, and other environmental forces resume immediately unless another Normal Technique takes authority.

Terrain remains mechanically authoritative and Normal Techniques do not continue pushing against or passing through solid geometry.

## Primed Technique Terrain Collision

A Primed Technique that contacts solid terrain transitions into its authored Primed terrain-impact response.

It does not pass through terrain or immediately return to ordinary control.

The Primed Technique retains authority through the impact response and any authored recovery.

Every Primed Technique must resolve terrain contact clearly.

Only Techniques whose geometry naturally supports it require a useful or advantageous terrain interaction.

Useful terrain interactions should not be added solely to ensure that every Primed Technique has one.

## Vulnerability

Techniques grant no inherent invulnerability frames.

Irys remains vulnerable throughout Normal and Primed Techniques unless a specific Technique explicitly avoids, deflects, or negates an attack.

Primed interruption resistance prevents ordinary hits from cancelling the Technique. It does not prevent damage.

Any Technique-specific defensive property must be explicit rather than implied by Technique activation.

There is no universal post-hit invulnerability.

There is no Technique or Prime invulnerability.

## Enemy Body Collision

Enemy bodies block Irys during ordinary movement.

Ordinary movement uses firm kinematic body blocking against enemies.

Neither Irys nor the enemy passively pushes the other through overlap.

Passive enemy-body contact is harmless.

Enemy bodies do not physically obstruct Irys while a Normal or Primed Technique has authority.

Active Techniques ignore ordinary enemy blocking.

During a Technique, Irys follows the Technique’s authored path through enemies rather than stopping against their collision bodies.

Solid terrain remains blocking and resolves according to the applicable Normal or Primed terrain-collision rules.

An enemy that can physically stop a Technique must possess an explicit exceptional property. Technique obstruction is not implied by enemy size or ordinary body collision alone.

## Technique Orientation

A Technique’s mechanical direction takes effect immediately upon activation.

Movement direction, attack geometry, hit detection, and other mechanical orientation use the incoming Technique’s direction without waiting for animation.

Presentation must orient Irys and Ion toward the new trajectory clearly and convincingly.

Animation may interpolate, turn, or use transitional poses, but it must not delay the Technique’s movement, attack timing, hit geometry, or control.

The exact body and sword orientation for vertical Techniques is a presentation decision. It must make the direction of travel and attack readable without imposing additional mechanical delay.

## Per-Activation Hit Registration

Each Technique activation may hit a given target once unless that Technique is explicitly authored as multi-hit.

A single Technique activation may hit multiple different targets.

Continued overlap with the same target across multiple frames does not produce additional hits.

Full separation from the target’s collision volume makes that target eligible for a future fresh Technique activation.

The additional clearance distance beyond the target is visual only and is not required for renewed eligibility.

A fresh Technique activation that begins while Irys is still overlapping a target does not immediately hit that target again.

The fresh activation must first fully separate from that target and then make new contact before it may hit the target.

This prevents repeated left-right Technique activation from producing endless damage while Irys remains inside a large enemy.

Even after full separation and re-entry, a single Technique activation may not hit the same target twice unless that Technique is explicitly authored as multi-hit.

## Contact-Adjusted Normal Technique Travel

Advance, Ascend, and Descend each have an authored default endpoint and fixed authored duration.

Enemy contact does not stop these Techniques.

If a default endpoint would leave Irys inside the contacted enemy or would fail to make her traversal through that enemy visually clear, the Technique extends its endpoint.

The adjusted endpoint lies beyond the enemy’s far edge in the Technique’s direction of travel, plus a small fixed separation distance.

This separation exists to make it clear that Irys passed completely through the enemy. It does not increase damage, change hit eligibility, or provide a separate momentum bonus.

The Technique uses whichever endpoint is farther along its direction of travel:

- its normal free-space endpoint; or
- the contact-adjusted endpoint required to clear the enemy.

The Technique’s original authored duration is preserved at all costs.

When contact extends the endpoint, the remaining motion is recalculated immediately so Irys reaches the adjusted endpoint within the original completion time.

The required travel speed may increase without a cap.

There is no timing overrun, minimum clearance duration, or speed limit added to preserve visual moderation.

If the adjusted path contacts another enemy, the endpoint may extend again and the remaining motion is recalculated again within the same original completion time.

This can produce an extreme but intentional burst through very large or closely grouped enemies.

Technique input remains available during contact-adjusted clearance movement.

A new Normal Technique may immediately take authority during that movement according to the standard Normal Technique redirection rules.

Solid terrain remains authoritative. If the adjusted path reaches solid terrain, the Normal Technique terrain-collision rules apply.

## Implementation Priorities

The initial implementation should favor direct, readable behavior over additional exception systems.

Required implementation properties are:

- Normal Techniques respond immediately and remain interruptible throughout.
- Primed Techniques remain committed through their authored completion and recovery.
- Damage is contact-driven.
- Enemy contact never pauses Normal Technique movement.
- Repeated damage requires a fresh activation, full separation, and renewed contact.
- Contact-adjusted travel preserves each Normal Technique’s fixed duration.
- Ordinary movement never overrides an active Technique.
- Solid terrain remains authoritative.
- Enemy reactions clearly communicate successful contact.
- Exact acceleration curves, exit velocities, visual effects, and presentation transitions remain tunable through playtesting.

Do not introduce general input buffering, universal hit-stop, inherent invulnerability frames, universal stagger values, procedural impact timing, or additional resource restrictions to solve behavior already governed by this specification.
