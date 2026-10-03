---
name: to-spec
description: Create an implementation plan for a task and save it as a kebab-case Markdown
file under tmp, prefixed with the task code.
---

# To Spec

Use this skill when the user asks for an implementation plan, task plan, or specification for a repository task.

## Workflow

1. Identify the task code from the request or the repository task naming, such as `T1`. Normalize it to lowercase for the filename, such as `t1`.
2. Read the relevant repository context before planning, prioritizing `AGENTS.md`,
   `docs/product/product-spec.md`, `docs/architecture/architecture.md`, `docs/roadmap/roadmap.md`,
   and related plans under `tmp/`.
3. Determine a concise kebab-case filename that starts with the task code and describes the task.
   Use the form `tmp/<task-code>-<short-description>.md`, for example
   `tmp/t1-waveform-capture-plan.md`.
4. Write the implementation plan to that Markdown file. Do not implement the task unless the user separately asks for implementation.
5. Make the plan actionable and repository-specific. Include, as applicable:
   - Goal and non-goals
   - Scope and affected components
   - Implementation steps in dependency order
   - Interfaces, data flow, and important design decisions
   - Testing and verification plan
   - Documentation or configuration updates
   - Risks, assumptions, and open questions
   - Exit criteria
6. Keep the plan focused on the requested task, align it with existing architecture and roadmap gates, and avoid inventing requirements not supported by repository context.

## File Requirements

- The output must be a Markdown file inside `tmp/`. This directory is intentionally ignored by
  Git so plans can be handed from a planning session to a fresh implementation session without
  becoming repository documentation.
- The filename must be lowercase kebab case.
- The filename must begin with the normalized task code followed by a hyphen.
- Preserve existing plans in `tmp/`; create a new file unless the user explicitly asks to update one.
- If the task code is missing or ambiguous, ask the user for it before creating the file.
