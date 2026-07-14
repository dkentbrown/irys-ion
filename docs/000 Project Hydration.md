# Irys+Ion — Project Hydration

Version: 2.1

---

# Purpose

This document is the primary onboarding document for any future ChatGPT session or human collaborator.

Its purpose is to communicate the project's current state, repository structure, development methodology, collaboration style, design philosophy, and immediate work so a new session can become productive with minimal loss of continuity.

Read this document before contributing to the project.

---

# Estimated Reading Time

Hydration document:
~10 minutes

Canonical specifications:
~45 minutes

Notebook:
As needed.

---

# Read This First

This repository is intentionally documentation-first.

Documentation exists for one reason:

**To maximize the probability that GPT-5.6 (Sol) can successfully implement the game in a single uninterrupted implementation session.**

The objective is **not** perfect documentation.

The objective is a successful implementation.

When those goals conflict, prefer the path that most directly advances implementation readiness.

---

# Project Overview

Title

Irys+Ion

Genre

Fast-paced 2D cyberpunk roguelite action platformer.

Technology

- C++
- Raylib
- GitHub repository:
  https://github.com/dkentbrown/irys-ion

Primary Objective

Produce a complete, internally consistent specification that can be handed to GPT-5.6 (Sol) for a one-shot implementation with no creative intervention during coding.

---

# Repository Structure

docs/

    design/
        Canonical game design specifications.

    narrative/
        Narrative specifications.

    technical/
        Technical implementation constraints.

    art/
        Visual direction.

    audio/
        Audio direction.

    notebooks/
        Incubation area for non-canonical ideas.

    000 Project Hydration.md
        This document.

    999 Sol Implementation Specification.md
        Final package delivered to Sol.

---

# Document Ownership

Every document owns exactly one concept.

Avoid duplication.

Reference the owning document instead of repeating information.

Current ownership:

000 Design Philosophy
    Immutable principles.

001 Vision
    Intended player experience.

002 Combat Model
    Conceptual combat architecture.

003 Combat Language Specification
    Rules of The Language.

004 Player Feel
    Responsiveness and feel.

005 Enemy Bible
    Enemy design.

006 Boss Bible
    Boss design.

007 Progression
    Roguelite systems.

008 World Bible
    World design.

009 Narrative
    Story and lore.

010 Technical
    Engineering constraints.

Notebook
    Candidate ideas only.

---

# Development Methodology

Whiteboard

↓

Discussion

↓

Agreement

↓

Candidate Canon

↓

Notebook

↓

Canonical Specification

↓

Implementation

Do not bypass stages.

---

# Repository First

The repository is the project's memory.

Chat history is temporary.

If an important decision is not represented somewhere inside the repository, treat it as non-existent.

---

# Collaboration Style

The assistant acts as Lead Systems Designer.

Preferred workflow:

- Ask one foundational question at a time.
- Build systems incrementally.
- Delay implementation details until governing abstractions exist.
- Push back when necessary.
- Prefer cohesion over novelty.
- Prefer elegant systems over larger numbers of systems.
- Once a decision is settled, treat it as stable unless a contradiction appears.
- Once a concept is sufficiently stable, promote it into its owning specification rather than continuing to whiteboard it.

Avoid large speculative drafts before sufficient whiteboarding has occurred.

---

# Team Roles

Creative Director

User

Lead Systems Designer

ChatGPT

Repository Engineer

Codex

Implementation Engineer

GPT-5.6 Sol

These responsibilities should remain distinct.

---

# Standing Project Laws

- Every document removes ambiguity.
- Every mechanic must solve a design problem.
- Repository First.
- One Owner Rule.
- Candidate Canon before Specification.
- Protect the North Star.
- Player mastery is more important than player power.
- The Language Always Answers.
- Techniques Are Scale Invariant.

---

# Bias Toward Progress

Do not expand project infrastructure simply because it would be useful.

New documents, workflows, notebooks, or process improvements should only be introduced if they materially improve the probability of a successful implementation.

When choosing between improving the framework and advancing the game specification, prefer advancing the game specification.

---

# Current Canon

Approved

- Design Philosophy
- Vision
- Combat Model
- Combat Language Specification

Currently Under Construction

- Player Feel Specification

---

# Current Design Direction

Recent breakthroughs include:

- Combat is evolving into a language rather than a move list.
- Techniques are named after motion.
- Techniques define trajectories and intent rather than animations.
- Enemy progression is treated as a curriculum.
- The Language exists both mechanically and within the narrative.
- Player mastery is canonical.
- The Creator invented The Language.
- Ion chose to leave the Creator.
- The Ark serves as the roguelite hub.
- The final confrontation represents preservation versus evolution.

Additional discoveries should enter the Design Notebook before becoming canonical.

---

# Current Work

Continue interactive design of:

`docs/design/004 Player Feel Specification.md`

`docs/design/003 Combat Language Specification.md` is the canonical definition of The Language. Revise it only when contradictions or implementation ambiguities are discovered.

Discover one concept at a time.

Notebook first.

Specification second.

Avoid implementation work until the specifications are sufficiently complete.

---

# Definition of Ready

The project is ready for Sol when:

- The canonical specifications are sufficiently complete to remove major ambiguity.
- The Sol Implementation Specification is complete.
- Remaining uncertainty consists primarily of tuning, balancing, polish, and implementation details.

At that point, stop expanding the documentation and begin the implementation experiment.

---

# Things To Avoid

Do not:

- reopen settled decisions without identifying a contradiction;
- introduce mechanics merely because they are interesting;
- bypass the notebook;
- duplicate concepts across specifications;
- optimize for feature count;
- prematurely solve engineering problems;
- treat chat history as canonical.
