---
name: skinos-component-doc
description: |
  Write and maintain skinos component interface Markdown docs after creating or
  changing a com/exe. Layout must match doc/com/land/auth.md (Overview,
  Configuration reference, API Reference +/-, Published Joint Events).
  Use when finishing a new component, adding/changing HE APIs or config, writing
  "<name>.md" next to component code, or when the user says "写组件文档",
  "接口文档", "component doc", "照着 auth.md".
  Mandatory after any new/changed component API surface — do not skip.
  Do NOT use for libskin C API book (skin.md) or SDK board config.
---

# Skinos Component Interface Documentation

**Mandatory:** After creating or changing a component’s public surface (HE APIs,
configuration, joint publish/subscribe), **write or update** the matching
English Markdown interface doc **before** treating the task as done.

Do **not** leave “docs later”. If the user only asked for code, still produce
the doc in the same turn (or ask once where to put it if the path is ambiguous).

Layout ground truth: **`doc/com/land/auth.md`**.  
Full rules + templates: [reference.md](reference.md).

## When this applies

| Trigger | Action |
|---------|--------|
| New `com` / `exe` under `project/<proj>/` | Create `<component>.md` |
| Add/rename/remove HE API or config keys | Update the same `.md` |
| Change published joint events | Update **Published Joint Events** |
| User asks only for docs | Write/update `.md` from code + auth layout |

Related: **skinos-project** scaffolds code; this skill owns the **interface doc**.

## Doc location & naming

1. Place **`<component-id>.md`** next to the component (same project tree; usually
   beside the com/exe directory or under the project’s agreed docs path).
2. Title id must be the real object name users call (e.g. `ipsec@client`,
   `land@auth`) — never leave `<组件名称>` placeholders.
3. Entire shipped doc is **English** (headings, body, tables, comments in samples).

## Workflow (do in order)

```
Doc progress:
- [ ] 1. Read doc/com/land/auth.md (layout) + this skill’s reference.md
- [ ] 2. Inventory from code: config keys, public APIs, joint publish/hook
- [ ] 3. Draft English .md in auth section order (omit empty optional sections)
- [ ] 4. Re-check blank lines, +/− API layout, JSON fences vs auth
- [ ] 5. Hard-ban pass (list below)
```

1. **Read** `doc/com/land/auth.md` and [reference.md](reference.md).
2. **Inventory** exported surface from source (`_setup` / `_shut` / custom APIs,
   config via `_get`/`_set`, joint names). Do **not** document `get`/`set` as
   user-facing APIs.
3. **Write** the `.md` using the section order below.
4. **Compare** blank-line counts and `+` / `-` punctuation to the closest auth
   analogue (hyphen runs, trailing spaces after `**` when auth has them).
5. **Ban check** — if any ban hits, fix before finishing.

## Required section order

Omit a section **entirely** when unused (no empty headings, no “N/A” prose).

| # | Heading | Required? |
|---|---------|-----------|
| 1 | `## <id> — <English subtitle>` (em dash `—`) | Yes |
| 2 | `### Overview` | Yes |
| 3 | `### Architecture` / `### Dependencies` / `### Joint Events Hook` | Optional |
| 4 | `### Configuration reference ( <id> )` + json + examples | **All three omit if no config** |
| 5 | `### Concepts` | Optional (before API Reference) |
| 6 | `### API Reference` → Management / Query / Control | Yes (empty subgroups: omit that `####`) |
| 7 | `### Published Joint Events` | Omit if none |
| 8 | `### Other` | Omit if empty |

Configuration trio (all or nothing):

- `### Configuration reference ( <id> )` — one space after `(` and before `)`
- `#### Configuration example`
- `#### Configuration settings example` (at least single-attr set + merge set)

## Hard bans (final doc)

- No teaching text / `//''` / `//'…'` from guides
- No `---` between major sections
- No `#####` per-API headings; APIs use `+` / `-` only
- No `#### Configuration attributes`
- No documenting `get` / `set` as user APIs
- No “this component has no configuration” — omit config sections instead
- No `*italic*` return blurb on the `+` line — returns only in `-` lines
- No `[param]` / `[[param]]` around optional args on the `+` line
- No skipping the `` ```json `` block after `- succeed return [ json ]`
- No replacing a JSON block with “same as X”
- Config JSON first line exactly: `// Attributes introduction ` (trailing space)
- Nested objects: `{ json }` / `[ string ]: { json }`, not bare `[ json ]` for object shape
- `#` comments in shell samples column-aligned with spaces

## API entry shape (auth style)

```markdown
+ `api[ param1, param2 ]` **one-line English summary**
    - param1 ----------- [ string ], …
    - param2 ----------- [ string ], optional, …
    - failed return NULL
    - succeed return [ json ], …

    ```json
    {
        "field": "…"  // [ string ], …
    }
    ```

    Example, …
    ```shell
    project@component.api[ value1, value2 ]
    { … }
    ```
```

- On the `+` line, mirror **auth**: method form like `` `domain[ username ]` ``
  (no `project@` prefix on the `+` line). Shell examples use `project@component.…`.
- Classify: lifecycle → **Management**; read-only → **Query**; mutating → **Control**.
- Types by **semantic intent**: counters/ports/PIDs → `[ number ]`; names/paths/units like `"256M"` → `[ string ]`.

## Done criteria

Task is incomplete until:

1. `<component-id>.md` exists at the agreed path, and  
2. It matches auth layout for every section that applies, and  
3. Hard bans pass.

## Additional resources

- [reference.md](reference.md) — blank-line targets, JSONC rules, full template, English checklist
- Layout sample: `doc/com/land/auth.md`
- Scaffolding code: **skinos-project** skill
