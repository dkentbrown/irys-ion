# Art Direction

Status: Draft

## Purpose

This document defines the visual identity and minimum art-production rules required to implement Irys+Ion coherently.

The objective is not to prescribe every asset in advance. It is to prevent the implementation from becoming visually generic, internally inconsistent, unreadable during combat, or dependent on an infeasible volume of bespoke character animation.

Combat is the game’s primary selling point. Visual style must support the speed, elegance, intensity, and readability of The Language.

## Core Visual Identity

Irys+Ion uses a blended visual approach:

- unmistakable pixel-art presentation;
- modular rigged character construction underneath;
- smooth high-intensity motion;
- authored poses and silhouettes;
- vivid luminous cyberpunk color against dark industrial mass.

The contrast between dark restraint and saturated color is part of the soul of the game.

Darkness and muted industrial material establish physical scale, age, and mass.

Vivid color communicates:

- active intelligence;
- intention;
- danger;
- Prime;
- dialogue signals;
- responsive machinery;
- important interactable systems.

The game should not choose between dark industrial imagery and colorful cyberpunk imagery. Its identity comes from their collision.

## Internal Resolution

The baseline internal render resolution is:

- 640 × 360;
- 16:9;
- nearest-neighbor scaling;
- integer scaling where practical;
- letterboxing or pillarboxing preferred over uneven pixel distortion.

This resolution must support:

- wide combat framing;
- large boss attack geometry;
- readable Irys silhouettes;
- crisp Ion trails;
- visible synthetic anatomy;
- stable HUD presentation.

Selection interfaces and rare full-screen scenes may use larger dedicated pixel-art assets rather than enlarging the normal gameplay sprite.

## Pixel Rendering

Use crisp pixel clusters and restrained shading.

Characters and combat-critical objects may use selective dark contouring for readability, but the game should not place thick black outlines around every internal form.

Separate materials through:

- value;
- hue;
- highlight shape;
- edge contrast;
- surface texture.

Synthetic skin should appear smoother and more luminous than armor.

Armor, machinery, and structural components should use sharper planes and harder highlights.

Bright effects may bloom selectively, but must resolve back into clean pixels rather than remaining as soft, high-resolution graphics layered over pixel art.

## Animation Standard

Gameplay movement, camera movement, and combat must appear smooth at the target frame rate.

The art direction does not impose a fixed low animation sampling rate.

Character animation may use:

- smooth rig movement;
- stepped authored poses;
- sprite substitution;
- alternate angle sprites;
- controlled deformation;
- smear shapes;
- trails;
- impact poses;
- selective interpolation.

The result must not resemble loosely rotating paper-doll pieces.

Technique movement must look:

- fluid;
- deliberate;
- intense;
- elegant;
- readable.

Pixel-art convention is subordinate to whether The Language looks and feels exceptional.

## Irys Body Construction

Each playable Irys body is an adult female synthetic construct manufactured by the Ark.

Bodies must remain visibly synthetic rather than appearing to be ordinary humans wearing futuristic clothing.

Manufactured features may include:

- segmented body plates;
- visible seams;
- artificial joints;
- temple hardware;
- interface ports;
- serial markings;
- embedded luminous channels;
- synthetic musculature;
- manufactured facial geometry;
- artificial eyes.

Bodies vary through the progression system’s authored anatomy and statistical properties.

Visual variation must support:

- stature;
- mass;
- limb length;
- missing limbs;
- additional limbs;
- asymmetry;
- altered balance;
- altered reach;
- reinforced or lightweight construction;
- other approved anatomy traits.

Every generated body must remain visually coherent and capable of expressing The Language.

## Modular Gameplay Rig

Gameplay Irys sprites are assembled from modular pixel-art components around a lightweight rig.

Expected component categories include:

- head;
- synthetic hair;
- upper torso;
- lower torso;
- pelvis;
- upper arms;
- lower arms;
- hands;
- thighs;
- lower legs;
- feet;
- alternate anatomy parts;
- shared Ark-suit components;
- Ion attachment points.

The rig controls placement and movement.

Authored poses, alternate sprites, deformation, and effects preserve strong pixel silhouettes.

Generated body dimensions may alter the rig and part placement, but must not create malformed or unreadable results.

## Inspection Presentation

Ark selection and post-boss substrate-selection interfaces provide the closest views of generated Irys bodies.

These scenes may be the only time vivid facial and material details are clearly visible.

Inspection presentation should support:

- full-body viewing;
- front view;
- three-quarter view;
- side view;
- rear or rear three-quarter view;
- facial inspection;
- anatomy-trait inspection;
- suit variation;
- synthetic seams and markings;
- visible damage and remaining structural integrity.

Gameplay and inspection assets represent the same generated body but do not need to be literal scaled versions of the same sprite.

Higher-detail modular inspection assets are permitted.

## Shared Ark Suit

All playable Irys bodies share one recognizable Ark-manufactured suit vocabulary.

The suit is primarily:

- an athletic synthetic bodysuit;
- integrated with selective structural armor;
- manufactured with the body rather than dressed onto it afterward.

Hard structural pieces should concentrate around areas such as:

- collar;
- sternum;
- hips;
- forearms;
- lower legs.

A recognizable collar-to-sternum structure should serve as a franchise-level costume signature.

The suit adapts to anatomy rather than hiding it.

It must preserve the readability of:

- extra limbs;
- missing structures;
- asymmetry;
- elongated proportions;
- unusual stature;
- generated body shape.

The suit should never become bulky enough to erase the body-generation system.

## Femininity, Fan Service, and Cosplay

A core visual success test is:

> An Irys should be visually distinctive enough that someone would want to cosplay her.

Irys bodies should use tasteful, adult, stylized femininity.

Variation may include:

- feminine proportions;
- variable neckline and chest construction;
- some cleavage on selected bodies;
- hip and leg cuts;
- seam placement that emphasizes silhouette;
- glamorous or revealing suit variants;
- rear three-quarter presentation during inspection;
- occasional tasteful emphasis on hips or butt.

Fan service must remain:

- adult;
- non-explicit;
- stylized;
- cosplayable;
- subordinate to character identity and combat readability.

Not every Irys is equally glamorous.

Some bodies may be:

- utilitarian;
- severe;
- damaged;
- awkward;
- visibly defective;
- unusually elegant;
- strikingly attractive.

Appearance and mechanical strength are not required to correlate.

## Faces

Irys faces should be immediately recognizable as manufactured constructs while remaining feminine, attractive, and expressive.

Use realistic adult facial proportions rather than chibi or anime facial construction.

Do not use anime-styled or oversized eyes.

Faces should include believable:

- eye size and placement;
- eyelids;
- brow structure;
- nose;
- mouth;
- jaw;
- cheeks;
- expressions.

Synthetic construction should remain overt through:

- facial seams;
- segmented plates;
- artificial lenses;
- luminous pupils;
- aperture-like irises;
- temple hardware;
- serial marks;
- nonhuman surface materials.

Faces should be generated from curated authored structures rather than unconstrained random sliders.

Defects may affect finish, symmetry, or component quality without becoming grotesque.

## Hair

Irys hair is synthetic and engineered while remaining unmistakably readable as hair.

Use:

- geometric cuts;
- controlled strand groups;
- molded or segmented locks;
- vivid color blocks;
- luminous fibers;
- strong silhouettes;
- deliberate asymmetry.

Avoid realistic high-cost hair simulation.

Hair movement should be controlled and authored.

Hair must remain:

- recognizable at gameplay scale;
- distinctive during inspection;
- useful for differentiating frames;
- physically cosplayable.

## Body Horror

Use body horror lightly.

Ark-generated Irys bodies remain attractive, synthetic, and cosplayable.

Defects should read as:

- manufacturing variation;
- incomplete construction;
- asymmetry;
- component damage;
- unusual anatomy.

Avoid:

- gore;
- wet exposed tissue;
- sustained grotesque imagery;
- excessive biomechanical horror.

Planetary-AI experiments may be uncanny or subtly anatomically wrong.

The restrained proto-Irys boss may be disturbing through scale, restraint, and incompletion rather than gore.

Anti-Irys should be refined and unsettling rather than monstrous.

## Damage Presentation

Irys and Anti-Irys damage remains dry.

Use:

- fractured synthetic plates;
- separated panels;
- sparks;
- exposed internal light;
- disrupted luminous channels;
- damaged suit sections;
- disabled components.

Do not use:

- blood;
- coolant spray;
- wet synthetic fluid;
- realistic gore.

Damage must communicate mechanical severity without obscuring the character design.

## Ion

Ion is the persistent visual anchor across every Irys frame and every run.

Ion retains one fixed iconic design.

The blade is fundamentally a refined sword with subtly impossible construction.

Required visual traits:

- long, elegant, single-edged silhouette;
- narrow forward profile;
- offset or interrupted spine;
- luminous internal channel;
- minimal conventional guard;
- grip geometry designed for an Irys construct rather than an ordinary human;
- one unmistakable asymmetry visible at gameplay scale.

The current Irys body adapts its grip, stance, posture, and attachment points around Ion.

Ion’s identity does not change to match the generated body.

Prime completes, extends, or resolves part of Ion’s physical silhouette rather than merely making the existing blade brighter.

Temporary damage or narrative effects may alter Ion, but its identity must remain unmistakable.

## Color Identities

### Ion

Ion and player intent use:

- electric cyan;
- cyan-white;
- hot white;
- controlled violet during heightened Prime intensity.

### Ark and Irys Systems

The Ark and Irys use warmer synthetic colors such as:

- amber;
- coral;
- magenta;
- warm white.

These colors should feel alive and manufactured rather than natural or rustic.

### Planetary AI

The planetary AI uses:

- ultraviolet;
- toxic chartreuse;
- sterile white.

The planetary AI should feel inhumanly certain rather than emotionally angry.

Its color language is precise, invasive, computational, and cold.

### Damage and Emergency

Red remains available as a general warning, emergency, and critical-damage color.

It should not become the planetary AI’s primary identity.

### Environment

Neutral environments may use broader palettes but must never compete with:

- Ion;
- Prime;
- player health information;
- enemy telegraphs;
- boss attacks;
- essential interaction signals.

## Environmental Visual Arc

The megastructure begins as ancient, dark, dormant, and incomprehensibly vast.

Early regions emphasize:

- heavy industrial materials;
- enormous empty spaces;
- inactive systems;
- distant machinery;
- obscured purpose;
- sparse active color.

As the campaign advances, the megastructure becomes:

- more articulated;
- more luminous;
- more responsive;
- more maintained;
- more precise;
- visibly self-repairing;
- visibly manufacturing itself.

The final one or two megastructure regions should appear fully awake.

The control center is bright, immaculate, and active rather than merely darker than previous regions.

The environmental progression is:

1. Industrial perimeter.
2. Adaptive infrastructure.
3. Synthetic and body-form experimentation.
4. Refined control center.
5. Shutdown return.
6. Compromised Ark.

The shutdown return removes the planetary AI’s ultraviolet and chartreuse life from the facility.

The world should then feel like a massive machine whose coordinating intelligence has abruptly left.

## Enemies

Ordinary enemies remain visibly mechanical across all generations.

They may become:

- cleaner;
- more articulated;
- more agile;
- more precise;
- more fluent in directional movement.

Ordinary enemies must not become recognizably feminine or fully Irys-like.

Clearly feminine or Irys-like construction is reserved for:

- the restrained proto-Irys boss;
- Anti-Irys.

Enemy lineages should preserve recognizable silhouette inheritance across generations.

Primary silhouette families:

- Striker: forward-weighted, narrow, cutting or thrusting.
- Pursuer: low, mobile, articulated, spacing-oriented.
- Anchor: wide, rooted, architectural, lane-controlling.

## Boss Visual Direction

### First Guardian

Crude, industrial, and strongly non-Irys-like.

### Second Guardian

More articulated and adaptive, with early humanoid hints but no complete Irys resemblance.

### Restrained Proto-Irys

A large incomplete Irys-like figure attached to or integrated into its environment.

It should feel unsettling through:

- scale;
- restraint;
- incomplete manufacture;
- partial bodily resemblance.

Avoid heavy gore.

### Command Mech

The fourth boss is a tall knight-like bipedal command mech.

It should be:

- mechanically elegant;
- neutral or masculine in proportion;
- immaculate;
- heavily armored;
- visually worthy of a false final boss.

It may use integrated or projected cutting structures but does not wield a literal counterpart to Ion.

### Anti-Irys

Anti-Irys has one fixed iconic body design.

She is the planetary AI’s highly polished conception of an ideal Irys substrate.

She should be:

- the most conventionally attractive Irys-like figure;
- the most polished;
- the most deliberately fan-service-forward;
- highly cosplayable;
- elegant;
- adult;
- non-explicit;
- immediately synthetic.

She shares the broad Ark/Irys construction vocabulary while appearing sharper, cleaner, more symmetrical, and more deliberate.

Her palette uses the planetary AI’s ultraviolet, chartreuse, and sterile-white identity.

She is a perfected counterfeit rather than an Ark-generated individual.

Damage progressively fractures her perfect exterior and exposes internal light without turning her into a grotesque wreck.

## Planetary-AI Blade

The planetary AI’s blade manifests during Anti-Irys’s activation scene.

It is analogous to Ion but must remain visually distinct.

It should appear:

- colder;
- more symmetrical;
- more computational;
- too perfect;
- less personally worn or evolved than Ion.

The blade remains the planetary AI’s persistent embodied form after the control-center transfer.

After Anti-Irys falls, it rises from her body in direct visual parallel to Ion’s run-failure behavior.

## Ark Intelligence

Irys—the Ark intelligence—does not use a stable humanoid hologram.

A simple recurring emblem may identify Irys in:

- dialogue boxes;
- candidate-selection interfaces;
- diagnostics;
- Ark controls.

The emblem should remain simple and readable at pixel scale.

The primary life and personality of Irys is expressed through the Ark itself:

- lighting;
- pod behavior;
- barriers;
- machinery;
- doors;
- protective movement;
- environmental responses.

During the final fight, the Ark’s scripted attempts to protect the pods are the strongest physical expression of Irys before the ending.

## Camera

Gameplay framing should err toward zoomed out rather than zoomed in.

The camera must show enough arena geometry to support:

- rapid redirection;
- boss telegraphs;
- vertical Techniques;
- committed paths;
- deliberate positioning.

Irys must remain large enough that the elegance and readability of The Language are not lost.

Vivid facial details are not required during normal gameplay.

Close facial and body detail belongs primarily to:

- Ark candidate selection;
- post-boss substrate selection;
- rare full-screen scenes.

Avoid frequent or reactive camera zooming during combat.

Each boss fight uses one fixed authored zoom for the complete encounter.

Do not use:

- repeated zoom pulses;
- reactive camera pumping;
- frequent push-ins;
- dynamic scale changes tied to ordinary attacks.

Camera framing may change between scenes, rooms, encounter states, or phase-transition cutscenes.

## Combat Effects

Effects must reinforce the established combat hierarchy.

### Normal Techniques

Use:

- sharp brief Ion trails;
- clear directional paths;
- limited visual residue;
- readable contact effects.

### Prime Armed State

Use:

- a persistent luminous change on Ion;
- a steady readable state;
- controlled electrical or ring-like signaling;
- a small distinct expiry cue.

### Primed Techniques

Use:

- larger and denser effects;
- stronger silhouette transformation;
- longer visual residue;
- heightened intensity.

Primed effects must not obscure:

- Irys;
- Ion;
- enemy telegraphs;
- terrain;
- boss attack geometry.

### Contact

Successful hits should read primarily on the enemy through:

- slash marks;
- sparks;
- recoil;
- deformation;
- palette heat;
- component damage.

Do not use player-centered hit-stop or effects that interrupt the intended continuous choreography.

## HUD

The standard gameplay HUD remains visible at all times.

The only HUD element whose visibility changes is the boss health bar.

The HUD should appear as a compact Ark/Ion diagnostic overlay.

Persistent elements include:

- current structural integrity;
- Prime state;
- other essential body-status information required by the final implementation.

The HUD may animate when values change, but its layout and baseline visibility remain stable.

Use:

- thin synthetic geometry;
- luminous channels;
- diagnostic marks;
- warm Ark accents;
- cyan Ion accents.

Avoid oversized decorative frames that consume combat space.

## Cutscenes and Full-Screen Art

Most narrative presentation remains in-engine.

Use:

- existing environments;
- gameplay rigs;
- authored poses;
- fixed camera compositions;
- lighting changes;
- pauses;
- text dialogue;
- environmental movement.

A very small number of rare full-screen pixel-art scenes may be used for major fixed-subject moments.

Full-screen scenes must not depict the variable in-run Irys body.

Suitable fixed subjects include:

- boss-introduction compositions;
- the command mech reveal;
- Anti-Irys suspended in the control-center chamber;
- Anti-Irys activation;
- the planetary AI draining into the counterfeit blade;
- the planetary-AI blade attempting to escape;
- the Irys–Ion merger;
- opening pods during the ending;
- the recognizable unselected candidate awakening.

Full-screen scenes should remain stylistically integrated with the game and rare enough to feel exceptional.

## Implementation Standard

The initial implementation must not use colored rectangles or generic primitives as the final visual identity of major characters.

Simplification is acceptable.

Visual incoherence is not.

The implementation should include:

- cohesive pixel silhouettes;
- recognizable modular Irys construction;
- a persistent authored Ion design;
- three readable enemy lineages;
- distinct boss silhouettes;
- the established color hierarchy;
- clear combat effects;
- an integrated HUD;
- visually distinct Ark and megastructure environments.

## Scope Boundaries

The Art Direction does not require:

- a complete concept-art production bible;
- unique frame-by-frame animation for every possible Irys body;
- realistic hair simulation;
- procedural generation of unrestricted facial geometry;
- large numbers of unrelated outfits;
- full-screen illustrations for ordinary dialogue;
- heavy body horror;
- realistic gore;
- high-resolution non-pixel cinematic rendering;
- a separate cinematic animation pipeline.

Additional art systems should be introduced only when their absence would materially reduce combat readability, generated-body coherence, narrative clarity, or the game’s recognizable identity.
